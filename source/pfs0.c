#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "pfs0.h"
#include "util.h"


#define PFS0_MAGIC 0x30534650


void pfs0_get_header(pfs0_struct_ptr *ptr, InstallProtocal mode)
{
    memset(&ptr->header, 0, sizeof(pfs0_header_t));
    read_data_from_protocal(mode, &ptr->header, sizeof(pfs0_header_t), 0x0, ptr->file, NULL);
}

void pfs0_populate_file_table(pfs0_struct_ptr *ptr, InstallProtocal mode)
{
    ptr->file_table = malloc(ptr->file_table_size);
    memset(ptr->file_table, 0x0, ptr->file_table_size);
    read_data_from_protocal(mode,  ptr->file_table, ptr->file_table_size, ptr->file_table_offset, ptr->file, NULL);
}

void pfs0_populate_string_table(pfs0_struct_ptr *ptr, InstallProtocal mode)
{
    ptr->string_table = malloc(ptr->header.total_files * sizeof(pfs0_string_table_t));
    u_int8_t *data_temp = malloc(ptr->header.string_table_size);
    memset(ptr->string_table, 0, ptr->header.string_table_size);
    memset(data_temp, 0, ptr->header.string_table_size);

    read_data_from_protocal(mode, data_temp, ptr->header.string_table_size, ptr->string_table_offset, ptr->file, NULL);

    for (u_int32_t i = 0; i < ptr->header.total_files; i++)
    {
        size_t offset = ptr->file_table[i].name_offset;
        for (u_int8_t j = 0; ; j++, offset++)
        {
            ptr->string_table[i].name[j] = data_temp[offset];
            if (ptr->string_table[i].name[j] == 0x00)
            {
                printf("found string %s\n", ptr->string_table[i].name);
                break;
            }
        }
    }
    free(data_temp);
}

bool pfs0_check_valid_magic(u_int32_t magic)
{
    if (magic != PFS0_MAGIC)
    {
        print_message_loop_lock("\ngot wrong magic %x16\n", magic);
        return false;
    }
    return true;
}

void pfs0_populate_table_size_offsets(pfs0_struct_ptr *ptr)
{
    ptr->file_table_offset = sizeof(pfs0_header_t);
    ptr->file_table_size = ptr->header.total_files * sizeof(pfs0_file_table_t);
    ptr->string_table_offset = ptr->file_table_offset + ptr->file_table_size;
    ptr->raw_data_offset = ptr->string_table_offset + ptr->header.string_table_size;
}

size_t pfs0_get_total_raw_data_size(pfs0_struct_ptr *ptr)
{
    size_t total_size = 0;
    for (u_int32_t i = 0; i < ptr->header.total_files; i++)
        total_size += ptr->file_table[i].data_size;
    return total_size;
}

int pfs0_search_string_table(pfs0_struct_ptr *ptr, const char *search_name)
{
    for (u_int32_t position = 0; position < ptr->header.total_files; position++)
        if (strstr(ptr->string_table[position].name, search_name))
            return position;
    return -1;
}

bool pfs0_extract_file(pfs0_struct_ptr *ptr, int location, InstallProtocal mode)
{
    FILE *new_file = fopen(ptr->string_table[location].name, "wb");
    if (!new_file)
    {
        print_message_loop_lock("failed to create new file %s\n", ptr->string_table[location].name);
        return false;
    }

    u_int64_t curr_off = ptr->raw_data_offset + ptr->file_table[location].data_offset;
    size_t file_size = ptr->file_table[location].data_size;
    print_message_display("extracting %s: size %lu\n", ptr->string_table[location].name, file_size);

    for (size_t offset = 0, buf_size = _8MiB; offset < file_size; offset += buf_size)
    {
        if (offset + buf_size > file_size)
            buf_size = file_size - offset;
        u_int8_t *buf = malloc(buf_size);

        read_data_from_protocal(mode, buf, buf_size, curr_off + offset, ptr->file, NULL);
        fwrite(buf, buf_size, 1, new_file);
        free(buf);
    }

    fclose(new_file);
    return true;
}

bool pfs0_extract_all(pfs0_struct_ptr *ptr, InstallProtocal mode)
{
    for (u_int32_t i = 0; i < ptr->header.total_files; i++)
        if (!pfs0_extract_file(ptr, i, mode))
            return false;
    return true;
}

void pfs0_free_structs(pfs0_struct_ptr *ptr)
{
    if (ptr->file_table != NULL)    free(ptr->file_table);
    if (ptr->string_table != NULL)  free(ptr->string_table);
}