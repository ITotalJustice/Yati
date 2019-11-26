#ifndef _NCZ_H_
#define _NCZ_H_

#include <stdbool.h>
#include <stdint.h>

#include "nca.h"


typedef struct
{
    uint64_t magic;
    uint64_t total_sections;
} ncz_header_t;

typedef struct
{
    uint64_t    offset;
    uint64_t    decompressed_size;
    uint64_t    crypto_type;
    uint64_t    padding;
    uint8_t     crypto_key[0x10];
    uint8_t     crypto_counter[0x10];
} ncz_section_t;

typedef struct
{
    uint64_t    magic;
    uint8_t     version;
    uint8_t     type;
    uint8_t     padding;
    uint8_t     block_size_exponent;
    uint32_t    total_blocks;
    size_t      decompressed_size;
    uint32_t    *compressed_block_size_list;
} ncz_block_t;

typedef struct
{
    nca_header_t header;
    u8 encrypted[0x3C04];
} ncz_first_0x4000;

typedef struct
{
    nca_struct_t    nca;

    ncz_header_t    header;
    ncz_section_t   *sections;
    ncz_block_t     block;

    size_t          ncz_size; // size of the entire ncz.
    uint64_t        ncz_section_offset; // starting offset of the section tables.
    size_t          ncz_section_size; // size of the total section tables.
    uint64_t        ncz_data_offset; // offset in encrypted data.
    size_t          ncz_data_size; // size of uncompressed nca - 0x4000.
} ncz_structs_t;


// get the header of the ncz. returns false if magic doesn't match.
void ncz_get_header(ncz_structs_t *ptr);

// populate the sizes / offsets of section and ncz data.
void ncz_populate_sizes_offsets(ncz_structs_t *ptr);

// populate the ncz sections.
void ncz_populate_sections(ncz_structs_t *ptr);

// check if the ncz magic is valid.
// returns 1 if valid.
bool ncz_check_valid_magic(ncz_structs_t *ptr);

// free structs and close file.
// checks if file exists and if data is used before freeing.
void ncz_free_structs(ncz_structs_t *ptr);

// open the ncz file.
void ncz_start_install(const char *name, size_t size, u64 offset, NcmStorageId storage_id, InstallProtocal mode, FILE *f);

#endif