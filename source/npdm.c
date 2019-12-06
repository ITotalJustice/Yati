#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "npdm.h"
#include "util.h"


bool npdm_check_if_meta_magic_valid(uint32_t magic)
{
    return magic_check(magic, META_MAGIC);
}

bool npdm_check_if_acid_magic_valid(uint32_t magic)
{
    return magic_check(magic, ACID_MAGIC);
}

bool npdm_check_if_aci0_magic_valid(uint32_t magic)
{
    return magic_check(magic, ACI0_MAGIC);
}

void npdm_get_aci0(npdm_ptr_t *ptr, install_protocal_t *install_protocal)
{
    memset(&ptr->aci0, 0, sizeof(npdm_aci0_t));
    read_data_from_protocal(&ptr->aci0, sizeof(npdm_aci0_t), ptr->meta.aci0_offset, install_protocal);
}

void npdm_get_acid(npdm_ptr_t *ptr, install_protocal_t *install_protocal)
{
    memset(&ptr->acid, 0, sizeof(npdm_acid_t));
    read_data_from_protocal(&ptr->acid, sizeof(npdm_acid_t), ptr->meta.acid_offset, install_protocal);
}

void npdm_get_meta(npdm_ptr_t *ptr, uint64_t offset, install_protocal_t *install_protocal)
{
    memset(&ptr->meta, 0, sizeof(npdm_meta_t));
    read_data_from_protocal(&ptr->meta, sizeof(npdm_meta_t), offset, install_protocal);
}