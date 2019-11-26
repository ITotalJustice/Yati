#ifndef _HFS0_H_
#define _HFS0_H_


#include <stdio.h>      // you already know what this is for...
#include <stdbool.h>    // for bools.
#include <stdint.h>     // for u_int64_t etc etc.

#include "util.h"

#define HFS0_MAGIC              0x30534648  // HFS0
#define HFS0_HEADER_OFFSET      0xF000


typedef struct
{
    u_int32_t magic;
    u_int32_t total_files;
    u_int32_t string_table_size;
    u_int32_t padding; 
} hfs0_header_t;

typedef struct
{
    u_int64_t data_offset;
    u_int64_t data_size;
    u_int32_t name_offset;
    u_int32_t hash_size;
    u_int64_t padding;
    char hash[0x20]; // i think i use a char? maybe use an u_int_t[0x20]???
} hfs0_file_table_t;

typedef struct
{
    char name[256];
} hfs0_string_table_t;

typedef struct
{
    FILE *f;
    hfs0_header_t header;
    hfs0_file_table_t *file_table;
    hfs0_string_table_t *string_table;

    u_int64_t file_table_offset;
    size_t file_table_size;
    u_int64_t string_table_offset;
    u_int64_t raw_data_offset;
    size_t raw_data_size;
} hfs0_structs_t;


// get the entire header.
void hfs0_get_header(hfs0_structs_t *ptr, u64 offset, InstallProtocal mode);

// populate the file table.
void hfs0_populate_file_table(hfs0_structs_t *ptr, InstallProtocal mode);

// populate the string table.
void hfs0_populate_string_table(hfs0_structs_t *ptr, InstallProtocal mode);

//
bool hfs0_check_if_magic_valid(u_int32_t magic);

// 
void hfs0_populate_table_size_offsets(hfs0_structs_t *ptr, u64 offset);

// return the total size of all nca's.
size_t hfs0_get_total_raw_data_size(hfs0_structs_t *ptr);

// return the location of the found string, or return -1 if not found.
int hfs0_search_string_table(hfs0_structs_t *ptr, const char *search_name);

//
bool hfs0_extract_file(hfs0_structs_t *ptr, int location, InstallProtocal mode);

// extract all contents in a hfs0.
bool hfs0_extract_all(hfs0_structs_t *ptr, InstallProtocal mode);

// free all structs.
void hfs0_free_structs(hfs0_structs_t *ptr);

// start the extract all.
// this will begin getting the header and populating the tables.
void hfs0_start_extract(const char *file_name, NcmStorageId storage_id, InstallProtocal mode);

#endif