#ifndef _HFS0_H_
#define _HFS0_H_

#include <stdbool.h>
#include <stdint.h>

#include "util.h"

#define HFS0_MAGIC              0x30534648
#define HFS0_HEADER_OFFSET      0xF000


typedef struct
{
    uint32_t magic; // HFS0.
    uint32_t total_files;
    uint32_t string_table_size;
    uint8_t _0xC[0x4]; 
} hfs0_header_t;

typedef struct
{
    uint64_t data_offset;
    uint64_t data_size;
    uint32_t name_offset;
    uint32_t hash_size;
    uint64_t padding;
    uint8_t hash[0x20];
} hfs0_file_table_t;

typedef struct
{
    char name[256];
} hfs0_string_table_t;

typedef struct
{
    hfs0_header_t header;
    hfs0_file_table_t *file_table;
    hfs0_string_table_t *string_table;

    uint64_t file_table_offset;
    size_t file_table_size;
    uint64_t string_table_offset;
    uint64_t raw_data_offset;
    size_t raw_data_size;
} hfs0_ptr_t;


// get the entire header.
void hfs0_get_header(hfs0_ptr_t *ptr, u64 offset, install_protocal_t *install_protocal);

// populate the file table.
void hfs0_populate_file_table(hfs0_ptr_t *ptr, install_protocal_t *install_protocal);

// populate the string table.
void hfs0_populate_string_table(hfs0_ptr_t *ptr, install_protocal_t *install_protocal);

//
bool hfs0_check_if_magic_valid(uint32_t magic);

// 
void hfs0_populate_table_size_offsets(hfs0_ptr_t *ptr, u64 offset);

// return the total size of all nca's.
size_t hfs0_get_total_raw_data_size(hfs0_ptr_t *ptr);

// return the location of the found string, or return -1 if not found.
int hfs0_search_string_table(hfs0_ptr_t *ptr, const char *search_name);

//
bool hfs0_extract_file(hfs0_ptr_t *ptr, int location, install_protocal_t *install_protocal);

// extract all contents in a hfs0.
bool hfs0_extract_all(hfs0_ptr_t *ptr, install_protocal_t *install_protocal);

// free all structs.
void hfs0_free_structs(hfs0_ptr_t *ptr);

// start the extract all.
// this will begin getting the header and populating the tables.
void hfs0_start_extract(const char *file_name, NcmStorageId storage_id, install_protocal_t *install_protocal);

#endif