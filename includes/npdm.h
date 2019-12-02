#ifndef _NPDM_H_
#define _NPDM_H_

#include <switch.h>


typedef enum
{
    MMU_64BitInstruction                        = 0x0,
    MMU_64BitAddressSpace                       = 0x1,
    MMU_32BitAdressSpace                        = 0x2,
    MMU_32BitAdressSpaceWithoutReservedRegion   = 0x3,
    MMU_OptimiseMemoryAllocation                = 0x4
} MMU;

typedef enum
{
    ACIDFlags_ProductionFlag,
    ACIDFlags_UnqualifiedApproval,
    ACIDFlags_
} ACIDFlags;

typedef struct
{
    u32 magic;                  // META
    u8 _0x4[0x8];
    u8 mmu_flag;                // see MUU.
    u8 _0xD;
    u8 main_thread_prio;
    u8 main_thread_core;
    u8 _0x10[0x4];
    u32 system_resource_size;
    u32 version;
    u32 main_thread_stack_size;
    u8 application_id[0x10];
    u8 product_code[0x10];
    u8 _0x40[0x30];

    u32 aci0_offset;
    u32 aci0_size;
    u32 acid_offset;
    u32 acid_size;
} npdm_meta_t;

typedef struct
{
    u32 magic;
    u8 _0x4[0xC];
    u64 title_id;
    u8 _0x18[0x8];

    u32 fs_access_header_offset;
    u32 fs_access_header_size;
    u32 service_access_control_offset;
    u32 service_access_control_size;
    u32 kernel_access_control_offset;
    u32 kernel_access_control_size;

    u8 _0x38[0x8];
} npdm_aci0_t;

typedef struct
{
    u8 rsa_fixed_key[0x100];
    u8 rsa_npdm[0x100];
    u32 magic;
    u32 data_size;
    u8 _0x204[0x4];
    u32 flags;
    u64 title_id_min;
    u64 title_id_max;

    u32 fs_access_header_offset;
    u32 fs_access_header_size;
    u32 service_access_control_offset;
    u32 service_access_control_size;
    u32 kernel_access_control_offset;
    u32 kernel_access_control_size;

    u8 _0x238[0x8];
} npdm_acid_t;

typedef struct
{
    npdm_meta_t meta;
    npdm_aci0_t aci0;
    npdm_acid_t acid;
} npdm_structs_t;

#endif