#ifndef _XCI_H_
#define _XCI_H_

#include <stdio.h>
#include <stdint.h>
#include <switch.h>
#include "hfs0.h"


#define XCI_CERT_OFFSET 0x7000
#define XCI_INFO_OFFSET 0x190


typedef enum 
{
    CARTSIZE_1GB    = 0xFA,
    CARTSIZE_2GB    = 0xF8,
    CARTSIZE_4GB    = 0xF0,
    CARTSIZE_8GB    = 0xE0,
    CARTSIZE_16GB   = 0xE1,
    CARTSIZE_32GB   = 0xE2
} gamecard_size_t;

typedef enum
{
    GC_FIRMWARE_DEVELOPMENT = 0x00,
    GC_FIRMWARE_RETAIL_100  = 0x01,
    GC_FIRMWARE_RETAIL_400  = 0x02
} gamecard_firmware_version_t;

typedef enum
{
    GC_ACCESS_CONTROL_25MHZ = 0x00A10011,
    GC_ACCESS_CONTROL_50MHZ = 0x00A10010
} gamecard_access_control_t;

typedef enum
{
    COMPAT_GLOBAL   = 0x00,
    COMPAT_CHINA    = 0x01
} xci_region_compatibility_t;

typedef enum 
{
    UPDATE = 0x657461647075,
    NORMAL = 0x6C616D726F6E,
    SECURE = 0x657275636573,
} xci_partition;

typedef struct
{
    uint64_t    firmware_version;
    uint32_t    access_control;
    uint32_t    read_time_1;
    uint32_t    read_time_2;
    uint32_t    write_time_1;
    uint32_t    write_time_2;
    uint32_t    firmware_mode;
    uint32_t    cup_version;
    uint8_t     compatibility_type;
    uint8_t     _0x25[0x3];
    uint64_t    update_partition_hash;
    uint64_t    CUP_ID;
    uint8_t     _0x38[0x38];
} xci_info_t;

typedef struct
{
    uint8_t         header_sig[0x100];
    uint32_t        magic;
    uint32_t        secure_offset;
    uint32_t        backup_offset;
    uint8_t         title_key_dec_index;
    uint8_t         gamecard_size;
    uint8_t         header_version;
    uint8_t         gamecard_flags;
    uint64_t        package_ID;
    uint64_t        cart_data_size;
    char            reversed_iv[0x10];
    uint64_t        hfs0_offset;
    uint64_t        hfs0_header_size;
    char            hfs0_header_hash[0x20];
    char            crypto_header_hash[0x20];
    uint32_t        security_mode;
    uint32_t        t1_key;
    uint32_t        key_index;
    uint32_t        end_address;
    char            encrypted_gamecard_info[0x70];
    xci_info_t      info;
} xci_header_t;

typedef struct
{
    uint8_t      cert_sig[0x100];
    uint32_t     magic;
    uint32_t     _0x104;
    uint8_t      kek_index;
    uint8_t      padding2[0x7];
    uint64_t     device_id;
    uint8_t      unkown[0xA];
    char        *encrypted_data[0xD0];
} xci_cert_t;

typedef struct
{
    uint64_t    package_id;
    uint8_t     _0x8[0x8];
    uint8_t     auth_data[0x10];
    uint8_t     auth_mac[0x10];
    uint8_t     auth_nonce[0x10];
    uint8_t     _0x2C[0x1C4];
} xci_initial_data_t;

typedef struct
{
    FILE            *f;         // only used if sd card install.

    hfs0_structs_t hfs0_root;   // root xci hfs0.
    hfs0_structs_t hfs0_update; // update partition.
    hfs0_structs_t hfs0_normal; // normal partition.
    hfs0_structs_t hfs0_secure; // secure partition.

    xci_header_t    header;     // see xci_header_t.
    xci_cert_t      cert;       // see xci_cert_t.
} xci_structs_t;


//
void xci_setup_install(const char *file_name, NcmStorageId storage_id, InstallProtocal mode);

#endif