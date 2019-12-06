#ifndef _XCI_H_
#define _XCI_H_

#include <stdio.h>
#include <stdint.h>
#include <switch.h>
#include "hfs0.h"


#define XCI_MAGIC       0x44414548 // HEAD.
#define XCI_CERT_MAGIC  0x47275636 // CERT.


#define XCI_UPDATE  "update"
#define XCI_NORMAL  "normal"
#define XCI_SECURE  "secure"

#define XCI_HEADER_SIZE 0x200
#define XCI_INFO_SIZE   0x70
#define XCI_CERT_SIZE   0x1F2

#define XCI_INFO_OFFSET 0x190
#define XCI_CERT_OFFSET 0x7000


typedef enum 
{
    GameCardSize_1GB    = 0xFA,
    GameCardSize_2GB    = 0xF8,
    GameCardSize_4GB    = 0xF0,
    GameCardSize_8GB    = 0xE0,
    GameCardSize_16GB   = 0xE1,
    GameCardSize_32GB   = 0xE2
} GameCardSize;

typedef enum
{
    GameCardFwVersion_Development   = 0x00,
    GameCardFwVersion_Retail100     = 0x01,
    GameCardFwVersion_Retail400     = 0x02
} GameCardFwVersion;

typedef enum
{
    GameCardAccessControl_25MHZ = 0x00A10011,
    GameCardAccessControl_50MHZ = 0x00A10010
} GameCardAccessControl;

typedef enum
{
    GameCardFlag_AutoBoot                           = 0x0,
    GameCardFlag_RepairTool                         = 0x1,
    GameCardFlag_HistoryErase                       = 0x2,
    GameCardFlag_DifferentRegionCupToTerraDevice    = 0x3,
    GameCardFlag_DifferentRegionCupToRegionDevice   = 0x4
} GameCardFlag;

typedef enum
{
    RegionCompatability_Nomral  = 0x00,
    RegionCompatability_Terra   = 0x01
} RegionCompatability;

typedef enum
{
    GameCardPartitionc_Update    = 0x0,
    GameCardPartitionc_Normal    = 0x1,
    GameCardPartitionc_Secure    = 0x2,
    GameCardPartitionc_Logo      = 0x3
} GameCardPartition;

typedef enum 
{
    GameCardPartitionMagic_Update    = 0x657461647075,
    GameCardPartitionMagic_Normal    = 0x6C616D726F6E,
    GameCardPartitionMagic_Secure    = 0x657275636573,
    GameCardPartitionMagic_Logo      = 0x47275636
} GameCardPartitionMagic;

typedef struct
{
    uint64_t firmware_version; // see GameCardFwVersion.
    uint32_t access_control; // see GameCardAccessControl.
    uint32_t read_time_1;
    uint32_t read_time_2;
    uint32_t write_time_1;
    uint32_t write_time_2;
    uint32_t fw_mode;
    uint32_t cup_version;
    uint8_t compatibility_type; // see XciRegionCompatability
    uint8_t _0x25[0x3];
    uint64_t update_partition_hash;
    uint64_t cup_id;
    uint8_t _0x38[0x38];
} xci_info_t;

typedef struct
{
    uint8_t header_sig[0x100];
    uint32_t magic;
    uint32_t secure_offset;
    uint32_t backup_offset;
    uint8_t title_key_dec_index;
    uint8_t gamecard_size; // see GameCardSize.
    uint8_t header_version;
    uint8_t gamecard_flag; // see GameCardFlag.
    uint64_t package_ID;
    uint64_t cart_data_size;
    uint8_t reversed_iv[0x10];
    uint64_t hfs0_offset;
    uint64_t hfs0_header_size;
    uint8_t hfs0_header_hash[0x20];
    uint8_t crypto_header_hash[0x20];
    uint32_t security_mode;
    uint32_t t1_key; // always 2
    uint32_t key_index;
    uint32_t end_address;
    uint8_t encrypted_gamecard_info[0x70];
    xci_info_t info;
} xci_header_t;

typedef struct
{
    uint8_t cert_sig[0x100];
    uint32_t magic; // CERT.
    uint8_t _0x104[0x4];
    uint8_t kek_index;
    uint8_t _0x109[0x7];
    uint64_t device_id;
    uint8_t _0x120[0xA];
    uint8_t encrypted_data[0xD0];
} xci_cert_t;

typedef struct
{
    uint64_t package_id;
    uint8_t _0x8[0x8];
    uint8_t auth_data[0x10];
    uint8_t auth_mac[0x10];
    uint8_t auth_nonce[0x10];
    uint8_t _0x2C[0x1C4];
} xci_initial_data_t;

typedef struct
{
    xci_header_t header; // see xci_header_t.
    xci_cert_t cert; // see xci_cert_t.
    hfs0_structs_t hfs0_root;   // root xci hfs0.
    hfs0_structs_t hfs0_update; // update partition.
    hfs0_structs_t hfs0_normal; // normal partition.
    hfs0_structs_t hfs0_secure; // secure partition.
} xci_ptr_t;

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