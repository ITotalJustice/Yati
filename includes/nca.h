#ifndef _NCA_H_
#define _NCA_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "pfs0.h"
#include "util.h"


#define NCA_SECTOR_SIZE             0x200
#define NCA_HEADER_SIZE             0xC00
#define SECTION_ENTRY_TABLE_SIZE    0x10
#define SECTION_HEADER_SIZE         0x200

#define NCA0_MAGIC 0x3041434E
#define NCA2_MAGIC 0x3241434E
#define NCA3_MAGIC 0x3341434E

#define NCA_PROGRAM_LOGO_OFFSET     0x8000
#define NCA_META_CNMT_OFFSET        0xC20

typedef enum
{
    NcaIsEncrypted_Encrypted,
    NcaIsEncrypted_Decrypted
} NcaIsEncrypted;

typedef enum
{
    NcaEncrpytMode_Decrypt  = 0x0,
    NcaEncrpytMode_Encrypt  = 0x1
} NcaEncrpytMode;

typedef enum
{
    NcaDistributionType_System      = 0x0,
    NcaDistributionType_GameCard    = 0x1
} NcaDistributionType;

typedef enum
{
    NcaContentType_Program      = 0x0,
    NcaContentType_Meta         = 0x1,
    NcaContentType_Control      = 0x2,
    NcaContentType_Manual       = 0x3,
    NcaContentType_Data         = 0x4,
    NcaContentType_PublicData   = 0x5
} NcaContentType;

typedef enum
{
    NcaOldKeyGeneration_100     = 0x0,
    NcaOldKeyGeneration_Unused  = 0x1,
    NcaOldKeyGeneration_300     = 0x2,
} NcaOldKeyGeneration;

typedef enum
{
    NcaKeyGeneration_301        = 0x3,
    NcaKeyGeneration_400        = 0x4,
    NcaKeyGeneration_500        = 0x5,
    NcaKeyGeneration_600        = 0x6,
    NcaKeyGeneration_620        = 0x7,
    NcaKeyGeneration_700        = 0x8,
    NcaKeyGeneration_810        = 0x9,
    NcaKeyGeneration_900        = 0x0A,
    NcaKeyGeneration_Invalid    = 0xFF
} NcaKeyGeneration;

typedef enum
{
    NcaKeyAreaEncryptionKeyIndex_Application    = 0x0,
    NcaKeyAreaEncryptionKeyIndex_Ocean          = 0x1,
    NcaKeyAreaEncryptionKeyIndex_System         = 0x2
} NcaKeyAreaEncryptionKeyIndex;

typedef enum
{
    NcaFileSystemType_RomFS = 0x0,
    NcaFileSystemType_PFS0  = 0x1
} NcaFileSystemType;

typedef enum
{
    NcaHashType_PFS0    = 0x2,
    NcaHashType_RomFS   = 0x3
} NcaHashType;

typedef enum
{
    NcaEncryptionType_None      = 0x1,
    NcaEncryptionType_AesCtrOld = 0x2,
    NcaEncryptionType_AesCtr    = 0x3,
    NcaEncryptionType_AesCtrEx  = 0x4
} NcaEncryptionType;

typedef enum
{
    NcaDataType_Normal  = 0x0,
    NcaDataType_Ncz     = 0x1
} NcaDataType;

typedef struct
{
    uint32_t media_start_offset; // divided by 0x200.
    uint32_t media_end_offset;   // divided by 0x200.
    uint32_t _0x8;               // unkown.
    uint32_t _0xC;               // unkown.
} nca_section_table_entry_t;

typedef struct
{
    uint16_t always_2;                   // always 2.
    uint8_t fs_type;          // see NcaFileSystemType.
    uint8_t hash_type;              // see NcaHashType.
    uint8_t encryption_type;  // see NcaEncryptionType.
    uint8_t _0x5[0x3];                  // empty.
    pfs0_superblock_t pfs0_sb;
    union
    {
        pfs0_superblock_t pfs0_sb;
        //romfs_superblock_t romfs_sb;
        // anything else?????
    };
    uint8_t bktr_not_finished[0x100]; // not finished. is optional?
} nca_section_header_t;

typedef struct
{
    uint8_t sha256[0x20];
} nca_section_header_hash_t;

typedef struct
{
    uint8_t area[0x10];
} nca_key_area_t;

typedef struct
{
    uint8_t rights_id[0x10];
} rights_id_t;

typedef struct
{
    uint8_t rsa_fixed_key[0x100];
    uint8_t rsa_npdm[0x100]; // key from npdm.
    uint32_t magic;
    uint8_t distribution_type; // see NcaDistributionType.
    uint8_t content_type; // see NcaContentType.
    uint8_t old_key_gen; // see NcaOldKeyGeneration.
    uint8_t key_area_encryption_key_index; // see NcaKeyAreaEncryptionKeyIndex.
    size_t nca_size;
    uint64_t title_id;
    uint32_t context_id;
    uint32_t sdk_version;
    uint8_t key_gen; // see NcaKeyGeneration.
    uint8_t _0x221[0xF]; // empty.
    FsRightsId rights_id;

    nca_section_table_entry_t section_table[0x4];
    nca_section_header_hash_t section_header_hash[0x4];
    nca_key_area_t key_area[0x4];

    uint8_t _0x340[0xC0]; // empty. next nca_section_header_t.

    nca_section_header_t section_header[0x4];
} nca_header_t;

typedef struct
{
    uint64_t offset;
    uint64_t size;
} section_info_t; // poor naming choice, couldn't think of anything better for now.

typedef struct
{
    uint64_t block_offset;
    uint8_t crypto_key[0x10];
    uint8_t crypto_counter[0x10];
} nca_encryption_t;

typedef struct
{
    nca_header_t header;
    section_info_t section_info[0x4];
    uint8_t section_total;

    uint64_t offset;
    size_t data_size;
    uint8_t data_type; // see NcaDataType;

    ncm_install_struct_t ncm;
} nca_ptr_t;

#endif
