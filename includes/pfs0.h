#ifndef _PFS0_H_
#define _PFS0_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <switch/services/fs.h>

#include "util.h"


typedef struct
{
    uint32_t magic;
    uint32_t total_files;
    uint32_t string_table_size;
    uint32_t padding;
} pfs0_header_t;

typedef struct
{
    uint64_t data_offset;
    uint64_t data_size;
    uint32_t name_offset;
    uint32_t padding;
} pfs0_file_table_t;

typedef struct
{
    char name[256];
} pfs0_string_table_t;

typedef struct
{
    uint8_t master_hash[0x20];
    uint32_t block_size;
    uint32_t always_2;
    uint64_t hash_table_offset;
    uint64_t hash_table_size; 
    uint64_t pfs0_offset;
    uint64_t pfs0_size;
    uint8_t _0x48[0xF0];
} pfs0_superblock_t;

typedef struct
{
    FILE *file;
    pfs0_header_t header;
    pfs0_file_table_t *file_table;
    pfs0_string_table_t *string_table;

    uint64_t file_table_offset;
    size_t file_table_size;
    uint64_t string_table_offset;
    uint64_t raw_data_offset;
    size_t raw_data_size;
} pfs0_struct_ptr;


// get the entire header.
void pfs0_get_header(pfs0_struct_ptr *ptr, InstallProtocal mode);

// store all the file info.
void pfs0_populate_file_table(pfs0_struct_ptr *ptr, InstallProtocal mode);

// store all the file names.
void pfs0_populate_string_table(pfs0_struct_ptr *ptr, InstallProtocal mode);

// check if the magic is valid.
// should be called after getting the header.
// returns 1 if success.
bool pfs0_check_valid_magic(u_int32_t magic);

// get the size and offset of file / string and raw table.
// this should be called after getting the pfs0_header.
void pfs0_populate_table_size_offsets(pfs0_struct_ptr *ptr);

// get the total size of the files within the nsp.
// to be used with remote installs where the filesize cannot be easily achieved.
size_t pfs0_get_total_raw_data_size(pfs0_struct_ptr *ptr);

// search string table for file name, returns location or -1 if not found.
int pfs0_search_string_table(pfs0_struct_ptr *ptr, const char *search_name);

// extract the given file from the pfs0.
bool pfs0_extract_file(pfs0_struct_ptr *ptr, int location, InstallProtocal mode);

// extract all files.
bool pfs0_extract_all(pfs0_struct_ptr *ptr, InstallProtocal mode);

// free file table and string table.
void pfs0_free_structs(pfs0_struct_ptr *ptr);

#endif