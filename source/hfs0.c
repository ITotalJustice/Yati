#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "hfs0.h"
#include "util.h"


void hfs0_get_header(hfs0_ptr_t *ptr, uint64_t offset, install_protocal_t *install_protocal)
{
    memset(&ptr->header, 0, sizeof(hfs0_header_t));
    read_data_from_protocal(&ptr->header, sizeof(hfs0_header_t), offset, install_protocal);
}

void hfs0_populate_file_table(hfs0_ptr_t *ptr, install_protocal_t *install_protocal)
{
    ptr->file_table = malloc(ptr->file_table_size);
    memset(ptr->file_table, 0x0, ptr->file_table_size);
    read_data_from_protocal(ptr->file_table, ptr->file_table_size, ptr->file_table_offset, install_protocal);
}

void hfs0_populate_string_table(hfs0_ptr_t *ptr, install_protocal_t *install_protocal)
{
    ptr->string_table = malloc(ptr->header.total_files * sizeof(hfs0_string_table_t));
    uint8_t *data_temp = malloc(ptr->header.string_table_size);
    memset(ptr->string_table, 0, ptr->header.string_table_size);
    memset(data_temp, 0, ptr->header.string_table_size);

    read_data_from_protocal(data_temp, ptr->header.string_table_size, ptr->string_table_offset, install_protocal);

    for (uint32_t i = 0; i < ptr->header.total_files; i++)
    {
        size_t offset = ptr->file_table[i].name_offset;
        for (uint8_t j = 0; ; j++, offset++)
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

bool hfs0_check_if_magic_valid(uint32_t magic)
{
    return magic_check(magic, HFS0_MAGIC);
}

void hfs0_populate_table_size_offsets(hfs0_ptr_t *ptr, uint64_t offset)
{
    ptr->file_table_offset = offset + sizeof(hfs0_header_t);
    ptr->file_table_size = ptr->header.total_files * sizeof(hfs0_file_table_t);
    ptr->string_table_offset = ptr->file_table_offset + ptr->file_table_size;
    ptr->raw_data_offset = ptr->string_table_offset + ptr->header.string_table_size;
}

size_t hfs0_get_total_raw_data_size(hfs0_ptr_t *ptr)
{
    size_t total_size = 0;
    for (uint32_t i = 0; i < ptr->header.total_files; i++)
        total_size += ptr->file_table[i].data_size;
    return total_size;
}

uint32_t hfs0_get_total_string_found(hfs0_ptr_t *ptr, const char *search_name)
{
    uint32_t total = 0;
    for (uint32_t i = 0; i < ptr->header.total_files; i++)
        if (strstr(ptr->string_table[i].name, search_name))
            total++;
    return total;
}

uint32_t hfs0_get_total_absolute_string_found(hfs0_ptr_t *ptr, const char *search_name)
{
    uint32_t total = 0;
    for (uint32_t i = 0; i < ptr->header.total_files; i++)
        if (strcmp(ptr->string_table[i].name, search_name) == 0)
            total++;
    return total;
}

int hfs0_search_string_table_for_string(hfs0_ptr_t *ptr, uint32_t offset, const char *search_name)
{
    for (uint32_t position = offset; position < ptr->header.total_files; position++)
        if (strstr(ptr->string_table[position].name, search_name))
            return position;
    return -1;
}

int hfs0_search_string_table_for_absolute_string(hfs0_ptr_t *ptr, uint32_t offset, const char *search_name)
{
    for (uint32_t position = offset; position < ptr->header.total_files; position++)
        if (strcmp(ptr->string_table[position].name, search_name) == 0)
            return position;
    return -1;
}

bool hfs0_extract_file(hfs0_ptr_t *ptr, int location, install_protocal_t *install_protocal)
{
    FILE *new_file = fopen(ptr->string_table[location].name, "wb");
    if (!new_file)
    {
        print_message_loop_lock("failed to create new file %s\n", ptr->string_table[location].name);
        return false;
    }

    uint64_t curr_off = ptr->raw_data_offset + ptr->file_table[location].data_offset;
    size_t file_size = ptr->file_table[location].data_size;
    print_message_display("extracting %s: size %lu\n", ptr->string_table[location].name, file_size);

    for (size_t offset = 0, buf_size = DataSize_8MiB; offset < file_size; offset += buf_size)
    {
        if (offset + buf_size > file_size) buf_size = file_size - offset;
        void *buf = malloc(buf_size);

        read_data_from_protocal(buf, buf_size, curr_off + offset, install_protocal);
        fwrite(buf, buf_size, 1, new_file);
        free(buf);
    }

    fclose(new_file);
    return true;
}

bool hfs0_extract_all(hfs0_ptr_t *ptr, install_protocal_t *install_protocal)
{
    for (uint32_t i = 0; i < ptr->header.total_files; i++)
        if (!hfs0_extract_file(ptr, i, install_protocal))
            return false;
    return true;
}

void hfs0_free_structs(hfs0_ptr_t *ptr)
{
    if (ptr->file_table)
        free(ptr->file_table);
    if (ptr->string_table)
        free(ptr->string_table);
}