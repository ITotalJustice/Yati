#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "nca.h"
#include "ncm.h"
#include "cnmt.h"
#include "util.h"


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

void *nca_get_header(nca_struct_t *nca_struct)
{
    void *out = malloc(NCA_HEADER_SIZE);
    read_data_from_protocal(nca_struct->mode, out, NCA_HEADER_SIZE, 0, nca_struct->nca_file, nca_struct->nca_file2);
    return out;
}

void *nca_encrypt_ctr(void *out, const void *in, u8 *key, u8 *counter, size_t size, u64 offset)
{
    Aes128CtrContext ctx;
    aes128CtrContextCreate(&ctx, key, counter);

    u64 swp = __bswap64(offset >> 4);
    memcpy(&counter[0x8], &swp, 0x8);
    aes128CtrContextResetCtr(&ctx, counter);
    aes128CtrCrypt(&ctx, out, in, size);

    return out;
}

void *nca_encrypt_decrypt_xts(void *out, const void *in, u64 sector, size_t size, NcaEncrpytMode mode)
{
    u8 key[0x20] = { 0xAE, 0xAA, 0xB1, 0xCA, 0x08, 0xAD, 0xF9, 0xBE, 0xF1, 0x29, 0x91, 0xF3, 0x69, 0xE3, 0xC5, 0x67, 0xD6, 0x88, 0x1E, 0x4E, 0x4A, 0x6A, 0x47, 0xA5, 0x1F, 0x6E, 0x48, 0x77, 0x06, 0x2D, 0x54, 0x2D };
    u8 ip1[0x10] = { 0 };
    u8 ip2[0x10] = { 0 };

    memcpy(ip1, key, 0x10);
    memcpy(ip2, &key[0x10], 0x10);

    Aes128XtsContext ctx;
    aes128XtsContextCreate(&ctx, ip1, ip2, mode);

    for (u64 pos = 0; pos < size; pos += NCA_SECTOR_SIZE)
    {
        aes128XtsContextResetSector(&ctx, sector++, true);
        switch (mode)
        {
            case NCA_DECRYPT:
                aes128XtsDecrypt(&ctx, (u8 *)out + pos, (const u8 *)in + pos, NCA_SECTOR_SIZE);
                break;
            case NCA_ENCRYPT:
                aes128XtsEncrypt(&ctx, (u8 *)out + pos, (const u8 *)in + pos, NCA_SECTOR_SIZE);
                break;
        }
    }
    return out;
}

Result nca_single_thread_install(nca_struct_t *nca_struct)
{
    Result rc = 0;

    for (size_t offset = 0, buf_size = _8MiB; offset < nca_struct->nca_size; offset += buf_size)
    {
        if (offset + buf_size > nca_struct->nca_size)
            buf_size = nca_struct->nca_size - offset;
        void *data_temp = malloc(buf_size);

        print_message_display("\twriting file %ldMB - %ldMB\r", nca_struct->data_written / _1MiB, nca_struct->nca_size / _1MiB);

        read_data_from_protocal(nca_struct->mode, data_temp, buf_size, nca_struct->offset + offset, nca_struct->nca_file, nca_struct->nca_file2);
        rc = ncm_write_placeholder(&nca_struct->ncm.storage, &nca_struct->ncm.placeholder_id, &nca_struct->data_written, data_temp, buf_size);
        free(data_temp);

        if (R_FAILED(rc))
        {
            print_message_loop_lock("failed to install nca\n");
            break;
        }
    }

    return rc;
}

Result nca_register_placeholder(ncm_install_struct_t *ncm)
{
    Result rc = 0;
    if (ncm_check_if_nca_exists(&ncm->storage, &ncm->content_id))
        ncm_delete_nca(&ncm->storage, &ncm->content_id);
    if (R_FAILED(rc = ncm_register_placeholder(&ncm->storage, &ncm->content_id, &ncm->placeholder_id)))
        ncm_delete_placeholder(&ncm->storage, &ncm->placeholder_id);
    return rc;
}

Result nca_setup_placeholder(ncm_install_struct_t *ncm, const char *name, size_t size, NcmStorageId storage_id)
{
    Result rc = 0;

    // open ncm storage.
    if (R_FAILED(rc = ncm_open_storage(&ncm->storage, storage_id)))
        return rc;

    // generate a placeholder id.
    if (R_FAILED(rc = ncm_generate_placeholder_id(&ncm->storage, &ncm->placeholder_id)))
        return rc;

    // check if the placeholder exists, deletes it if found.
    if (ncm_check_if_placeholder_exists(&ncm->storage, &ncm->placeholder_id))
        ncm_delete_placeholder(&ncm->storage, &ncm->placeholder_id);

    // generate the content_id from the string name.
    ncm->content_id = nca_get_id_from_string(name);

    // finally create placeholder.
    if (R_FAILED(rc = ncm_create_placeholder(&ncm->storage, &ncm->content_id, &ncm->placeholder_id, size)))
        return rc;
    
    return rc;
}

void nca_set_distribution_type_to_system(nca_header_t *header)
{
    header->distribution_type = NcaDistributionType_System;
}

Result nca_start_install(const char *name, u64 offset, NcmStorageId storage_id, InstallProtocal mode, FILE *f, FsFile *f2)
{
    Result rc = 0;
    nca_struct_t nca_struct;

    // TODO VERIFY NCA.
    // should verify nca as well at this point.
    nca_header_t nca_header;
    read_data_from_protocal(mode, &nca_header, NCA_HEADER_SIZE, offset, f, f2);
    nca_encrypt_decrypt_xts(&nca_header, &nca_header, 0, NCA_HEADER_SIZE, NCA_DECRYPT);
    nca_header.distribution_type = 0;

    // setup the placeholder / get content_id.
    if (R_FAILED(rc = nca_setup_placeholder(&nca_struct.ncm, name, nca_header.nca_size, storage_id)))
    {
        print_message_loop_lock("failed to setup nca placeholder\n");
        return rc;
    }

    // fill in the needed data to be passed around.
    nca_struct.nca_file     = f;
    nca_struct.nca_file2    = f2;
    nca_struct.mode         = mode;
    nca_struct.nca_size     = nca_header.nca_size - NCA_HEADER_SIZE;
    nca_struct.offset       = offset + NCA_HEADER_SIZE;
    nca_struct.data_written = 0;

    // now that we have the nca size, we can setup the placeholder and write the header to it
    nca_encrypt_decrypt_xts(&nca_header, &nca_header, 0, NCA_HEADER_SIZE, NCA_ENCRYPT);
    ncm_write_placeholder(&nca_struct.ncm.storage, &nca_struct.ncm.placeholder_id, &nca_struct.data_written, &nca_header, NCA_HEADER_SIZE);

    // start nca install.
    if (R_FAILED(rc = nca_single_thread_install(&nca_struct)))
        print_message_loop_lock("failed to install nca");

    // register placeholder, cleanup after.
    if (R_FAILED(rc = nca_register_placeholder(&nca_struct.ncm)))
        print_message_loop_lock("failed to register nca");

    ncm_close_storage(&nca_struct.ncm.storage);
    return rc;
}

bool nca_prepare_single_install(const char *file_name, NcmStorageId storage_id)
{
    FILE *f = open_file(file_name, "rb");
    if (!f)
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

    for (int i = 1; i < cnmt_struct.total_cnmt_infos; i++)
    {
        char new_nca_name[FS_MAX_PATH] = {0};
        nca_get_string_from_id(cnmt_struct.cnmt_infos[i].content_id, new_nca_name);

        f = open_file(new_nca_name, "rb");
        if (!f)
            break;

        print_message_display("now installing %s\n", new_nca_name);

        nca_start_install(new_nca_name, 0, storage_id, SD_CARD_INSTALL, f, NULL);
        fclose(f);
    }

    free(cnmt_struct.cnmt_infos);
    return true;
}