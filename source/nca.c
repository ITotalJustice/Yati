#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "nca.h"
#include "ncm.h"
#include "util.h"
#include "crypto.h"


bool nca_check_if_magic_valid(uint32_t magic)
{
    switch (magic)
    {
        case NCA0_MAGIC:
            return true;
        case NCA2_MAGIC:
            return true;
        case NCA3_MAGIC:
            return true;
        default:
            return false;
    }
}

const char *nca_get_string_from_id(NcmContentId nca_id, char *nca_string_out)
{
    u64 nca_id_lower = __bswap64(*(u64 *)nca_id.c);
    u64 nca_id_upper = __bswap64(*(u64 *)(nca_id.c + 0x8));
    snprintf(nca_string_out, 0x21, "%016lx%016lx", nca_id_lower, nca_id_upper);
    return nca_string_out;
}

NcmContentId nca_get_id_from_string(const char *nca_in_string)
{
    NcmContentId nca_id     = {0};
    char lowerU64[0x11]     = {0};
    char upperU64[0x11]     = {0};
    memcpy(lowerU64, nca_in_string, 16);
    memcpy(upperU64, nca_in_string + 16, 16);
    *(u64 *)nca_id.c = __bswap64(strtoul(lowerU64, NULL, 16));
    *(u64 *)(nca_id.c + 8) = __bswap64(strtoul(upperU64, NULL, 16));
    return nca_id;
}

void nca_get_header(nca_header_t *header, uint64_t offset, install_protocal_t *protocal)
{
    read_data_from_protocal(header, NCA_HEADER_SIZE, offset, protocal);
}

void nca_get_header_decrypted(nca_header_t *header, uint64_t offset, install_protocal_t *protocal)
{
    nca_get_header(header, offset, protocal);
    nca_decrypt_header(header);
}

void nca_encrypt_header(nca_header_t *header)
{
    crypto_encrypt_decrypt_aes_xts(&header, &header, NULL, NULL, 0, NCA_SECTOR_SIZE, NCA_HEADER_SIZE, EncryptMode_Encrypt);
}

void nca_decrypt_header(nca_header_t *header)
{
    crypto_encrypt_decrypt_aes_xts(header, header, NULL, NULL, 0, NCA_SECTOR_SIZE, NCA_HEADER_SIZE, EncryptMode_Decrypt);
    nca_set_distribution_type_to_system(header);
}

void nca_decrypt_key_area()
{

}

void nca_encrypt_key_area()
{

}

void nca_set_actual_section_offsets_sizes(nca_ptr_t *ptr)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        if (ptr->section_header[i].always_2 != 2)
            break; // we've now got the total of sections.

        ptr->section_info[i].offset = media_to_offset(ptr->header.section_table[i].media_start_offset) + ptr->section_header[i].pfs0_sb.pfs0_offset;
        ptr->section_info[i].size =  media_to_offset(ptr->header.section_table[i].media_end_offset) - ptr->section_info[i].offset;
        ptr->section_total = i + 1;
    }
}

void nca_set_distribution_type_to_system(nca_header_t *header)
{
    header->distribution_type = NcaDistributionType_System;
}

Result nca_setup_placeholder(ncm_install_struct_t *out, size_t size, NcmContentId *content_id, NcmStorageId storage_id)
{
    Result rc = 0;

    // open ncm storage.
    if (R_FAILED(rc = ncm_open_storage(&out->storage, storage_id)))
        return rc;

    // generate a placeholder id.
    if (R_FAILED(rc = ncm_generate_placeholder_id(&out->storage, &out->placeholder_id)))
        return rc;

    // check if the placeholder exists, delete if found.
    if (ncm_check_if_placeholder_exists(&out->storage, &out->placeholder_id))
        ncm_delete_placeholder(&out->storage, &out->placeholder_id);

    // finally create placeholder.
    if (R_FAILED(rc = ncm_create_placeholder(&out->storage, memcpy(&out->content_id, content_id, sizeof(NcmContentId)), &out->placeholder_id, size)))
        return rc;
    
    return rc;
}

Result nca_register_placeholder(const ncm_install_struct_t *ncm)
{
    return ncm_register_placeholder(&ncm->storage, &ncm->content_id, &ncm->placeholder_id);
}

Result nca_install_encrypted(nca_ptr_t *ptr, install_protocal_t *protocal)
{
    Result rc = 0;

    for (uint64_t offset = 0, buf_size = DataSize_8MiB; offset < ptr->data_size; offset += buf_size)
    {
        if (offset + buf_size > ptr->data_size)
            buf_size = ptr->data_size - offset;
        void *data_temp = malloc(buf_size);

        print_message_display("\twriting file %ldMB - %ldMB\r", offset / DataSize_1MiB, ptr->data_size / DataSize_1MiB);

        read_data_from_protocal(data_temp, buf_size, ptr->offset + offset, protocal);

        if (ptr->data_type == NcaDataType_Ncz)
            //ncz_decompress(data_temp, buf_size);

        rc = ncm_write_placeholder(&ptr->ncm.storage, &ptr->ncm.placeholder_id, &ptr->offset, data_temp, buf_size);
        free(data_temp);

        if (R_FAILED(rc))
        {
            print_message_loop_lock("failed to install nca\n");
            break;
        }
    }

    return rc;
}

void nca_install_decrypted(nca_ptr_t *ptr, install_protocal_t *protocal)
{
    // encrypted nca...
    // then install encrypted.
    nca_install_encrypted(ptr, protocal);
}

void nca_setup_install(NcmContentId *content_id, size_t size, uint64_t offset, NcaDataType data_type, NcmStorageId storage_id, install_protocal_t *protocal)
{
    nca_ptr_t ptr;
    nca_get_header_decrypted(&ptr.header, offset, protocal);
    if (!nca_check_if_magic_valid(ptr.header.magic))
        return;

    nca_setup_placeholder(&ptr.ncm, ptr.header.nca_size, content_id, storage_id);
}

bool nca_prepare_single_install(const char *file_name, NcmStorageId storage_id)
{
    install_protocal_t install_protocal;
    install_protocal.std_file = open_file(file_name, "rb");
    if (!install_protocal.std_file)
        return false;
    
    nca_start_install(file_name, 0, storage_id, SD_CARD_INSTALL, f, NULL);
    fclose(f);

    if (!strstr(file_name, "cnmt.nca"))
        return true;

    cnmt_struct_t cnmt_struct;
    strcpy(cnmt_struct.cnmt_name, file_name);
    cnmt_struct.storage_id = storage_id;
    if (R_FAILED(cnmt_open(&cnmt_struct)))
        return true;

    free(cnmt_struct.cnmt_infos);
    return true;
}
