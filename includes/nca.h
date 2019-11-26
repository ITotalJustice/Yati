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

typedef struct
{
    u_int8_t          rsa_fixed_key[0x100];
    u_int8_t          rsa_npdm[0x100];
    u_int32_t         magic;
    u_int8_t          distribution_type;               // xci nca need to have this set to 0.
    u_int8_t          content_type;
    u_int8_t          old_key;
    u_int8_t          key_area_encryption_key_index;
    u_int64_t         nca_size;
    u_int64_t         title_id;
    u_int32_t         context_id;
    u_int32_t         sdk_version;
    u_int8_t          key_gen;
    u_int8_t          padding[0xF];
    FsRightsId        rights_id;

    u_int8_t          idk[0x40];
    u_int8_t          idk2[0x80];
    u_int8_t          key_area[0x40];

    u_int8_t          free[0x120];
} nca_header_t;

typedef struct
{
    InstallProtocal mode;               // see InstallProtocal.
    ncm_install_struct_t ncm;

    FILE *nca_file;                     // only used if mode == SD_CARD_INSTALL.
    size_t nca_size;                    // size of the nca.
    u_int64_t offset;                   // offset of the file.

    u_int8_t *data;                     // the data the nca is stoared into.
    size_t data_stored;                 // size of data currently stored into mem.
    size_t data_written;                // size of data written to placeholder.
} nca_struct_t;


// write into *nca_string_out the created string.
// return the string thats written.
const char *nca_get_string_from_id(const NcmContentId nca_id, char *nca_string_out);

// return the nca_id from the string.
NcmContentId nca_get_id_from_string(const char *nca_string);

// return the base title_id.
u_int64_t nca_get_base_id(u_int64_t title_id, NcmContentMetaType contentMetaType);

//
void *nca_encrypt_ctr(void *out, const void *in, u_int8_t *key, u_int8_t *counter, size_t size, u_int64_t offset);

//
void *nca_encrypt_decrypt_xts(void *out, const void *in, u_int64_t sector, size_t size, NcaEncrpytMode mode);

// get the header of the nca, returns the size read.
void *nca_get_header(nca_struct_t *nca_struct);

//
Result nca_register_placeholder(ncm_install_struct_t *ncm);

//
Result nca_setup_placeholder(ncm_install_struct_t *ncm, const char *name, size_t size, NcmStorageId storage_id);

// start installing the nca.
Result nca_start_install(const char *name, u_int64_t offset, NcmStorageId storage_id, InstallProtocal mode, FILE *f);

// installs a single nca.
// TODO: add support for cnmt full install.
bool nca_prepare_single_install(const char *file_name, NcmStorageId storage_id);

#endif