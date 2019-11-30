#ifndef _NCA_H_
#define _NCA_H_

#include <stdbool.h>
#include <switch.h>
#include "util.h"
#include "ncm.h"


#define NCA_SECTOR_SIZE 0x200
#define NCA_HEADER_SIZE 0x330


typedef enum
{
    NCA_DECRYPT,
    NCA_ENCRYPT
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

typedef struct
{
    u8 rsa_fixed_key[0x100];
    u8 rsa_npdm[0x100];
    u32 magic;
    u8 distribution_type;               // see NcaDistributionType.
    u8 content_type;                    // see NcaContentType.
    u8 old_key_gen;                     // see NcaOldKeyGeneration.
    u8 key_area_encryption_key_index;   // see NcaKeyAreaEncryptionKeyIndex.
    size_t nca_size;
    u64 title_id;
    u32 context_id;
    u32 sdk_version;
    u8 key_gen;                         // see NcaKeyGeneration.
    u8 padding[0xF];
    FsRightsId rights_id;

    u8 idk[0x40];
    u8 idk2[0x80];
    u8 key_area[0x40];
} nca_header_t;

typedef struct
{
    u16 version;            // always 2.
    u8 file_system_type;    // see NcaFileSystemType.
    u8 hash_type;           // see NcaHashType.
    u8 encryption_type;     // see NcaEncryptionType.
    u8 padding;
    u8 fs_super_block[0xF8];
} nca_section_header_t;

typedef struct
{
    InstallProtocal mode;               // see InstallProtocal.
    ncm_install_struct_t ncm;

    FsFile *nca_file2;
    FILE *nca_file;                     // only used if mode == SD_CARD_INSTALL.
    size_t nca_size;                    // size of the nca.
    u64 offset;                   // offset of the file.

    u8 *data;                     // the data the nca is stoared into.
    size_t data_stored;                 // size of data currently stored into mem.
    size_t data_written;                // size of data written to placeholder.
} nca_struct_t;


// write into *nca_string_out the created string.
// return the string thats written.
const char *nca_get_string_from_id(const NcmContentId nca_id, char *nca_string_out);

// return the nca_id from the string.
NcmContentId nca_get_id_from_string(const char *nca_string);

//
void *nca_encrypt_ctr(void *out, const void *in, u8 *key, u8 *counter, size_t size, u64 offset);

//
void *nca_encrypt_decrypt_xts(void *out, const void *in, u64 sector, size_t size, NcaEncrpytMode mode);

// get the header of the nca, returns the size read.
void *nca_get_header(nca_struct_t *nca_struct);

//
Result nca_register_placeholder(ncm_install_struct_t *ncm);

//
Result nca_setup_placeholder(ncm_install_struct_t *ncm, const char *name, size_t size, NcmStorageId storage_id);

//
void nca_set_distribution_type_to_system(nca_header_t *header);

// start installing the nca.
Result nca_start_install(const char *name, u64 offset, NcmStorageId storage_id, InstallProtocal mode, FILE *f, FsFile *f2);

// installs a single nca.
bool nca_prepare_single_install(const char *file_name, NcmStorageId storage_id);

#endif