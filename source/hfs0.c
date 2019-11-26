#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

#include "hfs0.h"
#include "cnmt.h"
#include "nca.h"
#include "util.h"


void hfs0_get_header(hfs0_structs_t *ptr, u64 offset, InstallProtocal mode)
{
    memset(&ptr->header, 0, sizeof(hfs0_header_t));
    read_data_from_protocal(mode, &ptr->header, sizeof(hfs0_header_t), offset, ptr->f);
}

void hfs0_populate_file_table(hfs0_structs_t *ptr, InstallProtocal mode)
{
    ptr->file_table = malloc(ptr->file_table_size);
    memset(ptr->file_table, 0x0, ptr->file_table_size);
    read_data_from_protocal(mode, ptr->file_table, ptr->file_table_size, ptr->file_table_offset, ptr->f);
}

void hfs0_populate_string_table(hfs0_structs_t *ptr, InstallProtocal mode)
{
    ptr->string_table = malloc(ptr->header.total_files * sizeof(hfs0_string_table_t));
    u_int8_t *data_temp = malloc(ptr->header.string_table_size);
    memset(ptr->string_table, 0, ptr->header.string_table_size);
    memset(data_temp, 0, ptr->header.string_table_size);

    read_data_from_protocal(mode, data_temp, ptr->header.string_table_size, ptr->string_table_offset, ptr->f);

    for (u_int32_t i = 0; i < ptr->header.total_files; i++)
    {
        size_t offset = ptr->file_table[i].name_offset;
        for (u_int8_t j = 0; ; j++, offset++)
        {
            ptr->string_table[i].name[j] = data_temp[offset];
            if (ptr->string_table[i].name[j] == 0x00)
            {
                print_message_display("found string %s\n", ptr->string_table[i].name);
                break;
            }
        }
    }
    free(data_temp);
}

bool hfs0_check_if_magic_valid(u_int32_t magic)
{
    if (magic != HFS0_MAGIC)
    {
        print_message_loop_lock("\ngot wrong magic %x16\n", magic);
        return false;
    }
    return true;
}

void hfs0_populate_table_size_offsets(hfs0_structs_t *ptr, u64 offset)
{
    ptr->file_table_offset = offset + sizeof(hfs0_header_t);
    ptr->file_table_size = ptr->header.total_files * sizeof(hfs0_file_table_t);
    ptr->string_table_offset = ptr->file_table_offset + ptr->file_table_size;
    ptr->raw_data_offset = ptr->string_table_offset + ptr->header.string_table_size;
}

size_t hfs0_get_total_raw_data_size(hfs0_structs_t *ptr)
{
    size_t total_size = 0;
    for (u_int32_t i = 0; i < ptr->header.total_files; i++)
        total_size += ptr->file_table[i].data_size;
    return total_size;
}

int hfs0_search_string_table(hfs0_structs_t *ptr, const char *search_name)
{
    for (u_int32_t position = 0; position < ptr->header.total_files; position++)
        if (strstr(ptr->string_table[position].name, search_name))
            return position;
    return -1;
}

bool hfs0_extract_file(hfs0_structs_t *ptr, int location, InstallProtocal mode)
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

    for (size_t offset = 0, buf_size = 0x800000; offset < file_size; offset += buf_size)
    {
        if (offset + buf_size > file_size) buf_size = file_size - offset;
        void *buf = malloc(buf_size);

        read_data_from_protocal(mode, buf, buf_size, curr_off + offset, ptr->f);
        fwrite(buf, buf_size, 1, new_file);
        free(buf);
    }

    fclose(new_file);
    return true;
}

bool hfs0_extract_all(hfs0_structs_t *ptr, InstallProtocal mode)
{
    for (u_int32_t i = 0; i < ptr->header.total_files; i++)
        if (!hfs0_extract_file(ptr, i, mode))
            return false;
    return true;
}

void hfs0_free_structs(hfs0_structs_t *ptr)
{
    if (ptr->f)             fclose(ptr->f);
    if (ptr->file_table)    free(ptr->file_table);
    if (ptr->string_table)  free(ptr->string_table);
}


void hfs0_start_extract(const char *file_name, NcmStorageId storage_id, InstallProtocal mode)
{
    // old xci install fucntion.
    // replaced this xci.c
}