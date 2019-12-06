#ifndef _NPDM_H_
#define _NPDM_H_

#include <stdint.h>


#define META_MAGIC 0x4154454D
#define ACID_MAGIC 0x44494341
#define ACI0_MAGIC 0x30494341

typedef enum
{
    MMUFlags_64BitInstruction                        = 0x0,
    MMUFlags_64BitAddressSpace                       = 0x1,
    MMUFlags_32BitAdressSpace                        = 0x2,
    MMUFlags_32BitAdressSpaceWithoutReservedRegion   = 0x3,
    MMUFlags_OptimiseMemoryAllocation                = 0x4
} MMUFlags;

typedef struct
{
    uint32_t magic; // META.
    uint8_t _0x4[0x8];
    uint8_t mmu_flag;   // see MMUFlags.
    uint8_t _0xD;
    uint8_t main_thread_prio;
    uint8_t main_thread_core;
    uint8_t _0x10[0x4];
    uint32_t system_resource_size;
    uint32_t version;
    uint32_t main_thread_stack_size;
    uint8_t title_name[0x10]; // usually Application.
    uint8_t product_code[0x10];
    uint8_t _0x40[0x30];

    uint32_t aci0_offset;
    uint32_t aci0_size;
    uint32_t acid_offset;
    uint32_t acid_size;
} npdm_meta_t;

typedef struct
{
    uint8_t rsa_fixed_key[0x100];
    uint8_t rsa_npdm[0x100];
    uint32_t magic; // ACID.
    uint32_t data_size;
    uint8_t _0x204[0x4];
    uint32_t flags;
    uint64_t title_id_min;
    uint64_t title_id_max;

    uint32_t fs_access_header_offset;
    uint32_t fs_access_header_size;
    uint32_t service_access_control_offset;
    uint32_t service_access_control_size;
    uint32_t kernel_access_control_offset;
    uint32_t kernel_access_control_size;

    uint8_t _0x238[0x8];
} npdm_acid_t;

typedef struct
{
    uint32_t magic; // ACI0.
    uint8_t _0x4[0xC];
    uint64_t title_id;
    uint8_t _0x18[0x8];

    uint32_t fs_access_header_offset;
    uint32_t fs_access_header_size;
    uint32_t service_access_control_offset;
    uint32_t service_access_control_size;
    uint32_t kernel_access_control_offset;
    uint32_t kernel_access_control_size;

    uint8_t _0x38[0x8];
} npdm_aci0_t;

typedef struct
{
    npdm_meta_t meta;
    npdm_aci0_t aci0;
    npdm_acid_t acid;
} npdm_ptr_t;

#endif