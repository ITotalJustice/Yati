#ifndef _CNMT_H_
#define _CNMT_H_

#include <stdint.h>
#include <stdbool.h>
#include <switch.h>


#define CNMT_HEADER_SIZE 0x20

typedef enum
{
    UpdateType_ApplyAsDelta = 0x0,
    UpdateType_Overwrite = 0x1,
    UpdateType_Create = 0x2
} UpdateType;

typedef enum
{
    StorageId_None = 0x0,
    StorageId_Host = 0x1,
    StorageId_GameCard = 0x2,
    StorageId_System = 0x3,
    StorageId_User = 0x4,
    StorageId_SdCard = 0x5,
    StorageId_Any = 0x6
} StorageId;

typedef enum
{
    ContentType_Meta = 0x0,
    ContentType_Program = 0x1,
    ContentType_Delta = 0x2,
    ContentType_Control = 0x3,
    ContentType_HtmlDocument = 0x4,
    ContentType_LegalInformation = 0x5,
    ContentType_DeltaFragment = 0x6
} ContentType;

typedef enum
{
    ContentMetaType_Invalid = 0x0,
    ContentMetaType_SysProgram = 0x1,
    ContentMetaType_SysData = 0x2,
    ContentMetaType_SysUpdate = 0x3,
    ContentMetaType_BootImagePackage = 0x4,
    ContentMetaType_BootImagePackageSafe = 0x5,
    ContentMetaType_Application = 0x80,
    ContentMetaType_Patch = 0x81,
    ContentMetaType_AddOnContent = 0x82,
    ContentMetaType_Delta = 0x83
} ContentMetaType;

typedef enum
{
    ContentMetaAttribute_None = 0x0,
    ContentMetaAttribute_IncludesExFatDriver = 0x1,
    ContentMetaAttribute_Rebootless = 0x2
} ContentMetaAttribute;

typedef enum
{
    ContentInstallType_Full = 0x0,
    ContentInstallType_FragmentOnly = 0x1,
    ContentInstallType_Invalid = 0x7
} ContentInstallType;

typedef struct
{
    uint8_t id[0x10];
} content_id_t;

typedef struct
{
    uint64_t title_id;
    uint32_t version;
    uint8_t meta_type;      // see ContentMetaType.
    uint8_t install_type;   // see ContentInstallType.
    uint8_t _0xE[0x2];
} content_meta_key_t;

typedef struct
{
    content_id_t content_id;
    uint8_t size[0x6];
    uint8_t content_type;   // see ContentType.
    uint8_t id_offset;
} content_info_t;

typedef struct
{
    uint16_t extended_header_size;
    uint16_t content_count;
    uint16_t content_meta_count;
    uint8_t attribute;
    uint8_t storage_id;
} content_meta_header_t;

typedef struct
{
    uint8_t hash[0x20];
    content_info_t info;
} content_record_t;

typedef struct
{
    uint32_t total;
    content_record_t *record; // *total.
} content_records_t;

typedef struct
{
    uint64_t title_id;
    uint32_t version;
    uint8_t meta_type;  // see ContentMetaType.
    uint8_t attribute;  // ContentMetaAttribute.
    uint8_t _0xE[0x2];
} content_meta_record_t;

typedef struct
{
    uint32_t extended_data_size;
} sys_update_extended_header_t;

typedef struct
{
    uint64_t patch_id;
    uint32_t required_system_version;
    uint32_t required_application_version;
} application_extended_header_t;

typedef struct
{
    uint64_t application_id;
    uint32_t required_system_version;
    uint32_t extended_data_size;
    uint8_t _0x10[0x8];  
} patch_extended_header_t;

typedef struct
{
    uint64_t application_id;
    uint32_t required_application_version;
    uint8_t _0xC[0x4]; 
} add_on_extended_header_t;

typedef struct
{
    uint64_t application_id;
    uint32_t extended_data_size;
    uint8_t _0xC[0x4];
} delta_extended_header_t;

typedef struct
{
    content_meta_record_t meta_record;
    uint8_t meta_type;  // // see ContentMetaType..
    uint8_t attribute;  // see ContentMetaAttribute.
    uint8_t _0xE[0x2];
    uint8_t _0x10[0x20];
    uint16_t content_info_count;
    uint8_t _0x32[0x6];
} history_header_t;

typedef struct
{
    uint64_t source_patch_id;
    uint64_t destition_patch_id;
    uint32_t source_version;
    uint32_t destition_version;
    uint64_t download_size;
    uint8_t _0x20[0x8];
} delta_history_t;

typedef struct
{
    uint64_t source_patch_id;
    uint64_t destition_patch_id;
    uint32_t source_version;
    uint32_t destition_version;
    uint16_t fragment_set_count;
    uint8_t _0x1A[0x6];
    uint16_t content_info_count;
    uint8_t _0x22[0x6];
} delta_header_t;

typedef struct
{
    uint8_t source_content_id[0x10];
    uint8_t destination_content_id[0x10];
    uint8_t source_size[0x6];
    uint8_t destination_size[0x8];
    uint16_t fragment_indicator_count;
    uint8_t content_type;   // see ContentType.
    uint8_t update_type;    // see UpdateType.
    uint8_t _0x30[0x4];
} fragment_set_t;

typedef struct
{
    uint16_t content_info_index;
    uint16_t fragment_index;
} fragment_indicator_t;

typedef struct
{
    uint32_t version;
    uint32_t veriation_count;
    uint32_t fw_variation;
} sys_update_extended_data_t;

typedef struct
{
    uint32_t history_count;
    uint32_t delta_history_count;
    uint32_t delta_count;
    uint32_t fragment_set_count;
    uint32_t history_set_count;
    uint32_t history_content_count;
    uint32_t delta_content_count;
    uint32_t _0x18[0x4];
    
    history_header_t *history_header;           // *history_count.
    delta_history_t *delta_history;             // *delta_history_count.
    delta_header_t *delta_header;               // *delta_count.
    fragment_set_t *fragment_set;               // *fragment_set_count.
    content_info_t *content_info;               // *history_content_count.
    content_record_t *delat_content_record;     // *delta_content_count;
    fragment_indicator_t fragment_indicator;    // *fragment_set_count.
} patch_extended_data_t;

typedef struct
{
    delta_header_t header;
    fragment_set_t *fragment_set;
    fragment_indicator_t *fragment_indicator;
} delta_extended_data_t;

typedef struct
{
    content_meta_key_t key;
    uint8_t storage_id;
} content_storage_record_t;

typedef struct 
{
    uint64_t title_id;
    uint32_t title_version;
    uint8_t meta_type; // see NcmContentType.
    uint8_t _0xD;
    content_meta_header_t meta_header;
    uint8_t _0x16[0x2];
    uint32_t required_sys_version;
    uint8_t _0x1C[4];
} cnmt_header_t;

typedef struct
{
    FsFileSystem system;
    FsDir dir;
    FsFile file;
} cnmt_storage_t;

typedef struct
{
    uint64_t total;
    content_id_t *id // *total.
} content_ids_t;

typedef struct
{
    uint8_t storage_id;
    uint64_t offset;
    cnmt_storage_t storage;
    cnmt_header_t header;
    uint8_t *extended_header;
    content_meta_key_t key;
    content_info_t *info;
} cnmt_ptr_t;


//
bool cnmt_start(content_ids_t *out, NcmContentId *content_id, uint8_t storage_id);

#endif