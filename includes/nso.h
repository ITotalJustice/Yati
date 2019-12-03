#ifndef _NSO_H_
#define _NSO_H_

#include <switch.h>

#define MAGIC_NSO 0x304F534E


typedef struct 
{
    u32 file_offset;
    u32 memory_offset;
    u32 decompressed_size;
} segment_header_t;

typedef struct
{
    u32 region_rodata_offset;
    u32 region_size;
} rodata_relative_extent_t;

typedef struct
{
    u8 sha256[0x20];
} segment_hash_t;

typedef struct
{
    u32 magic;
    u32 version;
    u8 _0x8[0x4]; // empty.
    segment_header_t text;
    u32 module_file_size;
    segment_header_t rodata;
    u32 module_file_size;
    segment_header_t data;
    u32 bss_size;
    u8 value_of_build_id[0x20];
    u32 text_compressed_size;
    u32 rodata_compressed_size;
    u32 data_compressed_size;
    u8 _0x6C[0x1C]; // empty.
    rodata_relative_extent_t api_info;
    rodata_relative_extent_t dynstr;
    rodata_relative_extent_t dynsym;
    segment_hash_t hashes[0x3];
} nso_header_t;

#endif