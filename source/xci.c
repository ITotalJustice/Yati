#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "hfs0.h"
#include "cnmt.h"
#include "xci.h"
#include "nca.h"
#include "ncz.h"
#include "util.h"


void xci_get_header(xci_ptr_t *ptr, install_protocal_t *install_protocal)
{
    memset(&ptr->header, 0, sizeof(xci_header_t));
    read_data_from_protocal(&ptr->header, sizeof(xci_header_t), 0, install_protocal);
}

bool xci_check_valid_head_magic(uint32_t magic)
{
    return magic_check(magic, XCI_MAGIC);
}

bool xci_check_valid_cert_magic(uint32_t magic)
{
    return magic_check(magic, XCI_CERT_MAGIC);
}

void xci_get_cert(xci_ptr_t *ptr, install_protocal_t *install_protocal)
{
    memset(&ptr->cert, 0, sizeof(xci_cert_t));
    read_data_from_protocal(&ptr->cert, sizeof(xci_cert_t), XCI_CERT_OFFSET, install_protocal);

}

void xci_decrypt_cert(xci_ptr_t *ptr)
{

}

void xci_dump_partition(GameCardPartition partition)
{

}

void xci_setup_install_new(const char *file, InstallProtocalMode mode)
{
    xci_ptr_t ptr;
    install_protocal_t install_protocal;
    install_protocal.mode = mode;
    if (mode == InstallProtocalMode_SD)
        if (!(install_protocal.std_file = open_file(file, "rb")))
            return;
    
    xci_get_header(&ptr);
    if (!xci_check_valid_head_magic(ptr.header.magic))
    {
        fclose(install_protocal.std_file);
        return;
    }

}

void xci_start_install(xci_structs_t *ptr, NcmStorageId storage_id, InstallProtocalMode mode)
{
    int loc = hfs0_search_string_table(&ptr->hfs0_secure, "cnmt.nca");
    if (loc == -1)
        return;

    if (R_FAILED(nca_start_install(ptr->hfs0_secure.string_table[loc].name, ptr->hfs0_secure.raw_data_offset + ptr->hfs0_secure.file_table[loc].data_offset, storage_id, mode, ptr->f, NULL)))
    {
        print_message_loop_lock("failed to install cnmt\n");
        return;
    }

    cnmt_struct_t cnmt_struct;
    cnmt_struct.storage_id = storage_id;
    strcpy(cnmt_struct.cnmt_name, ptr->hfs0_secure.string_table[loc].name);

    if (R_FAILED(cnmt_open(&cnmt_struct)))
    {
        print_message_loop_lock("\nfailed to open cnmt\n");
        return;
    }

    for (u_int8_t i = 1; i < cnmt_struct.total_cnmt_infos; i++)
    {
        char new_nca_name[FS_MAX_PATH] = {0};
        loc = hfs0_search_string_table(&ptr->hfs0_secure, nca_get_string_from_id(cnmt_struct.cnmt_infos[i].content_id, new_nca_name));
        if (loc == -1)
            break;

        // check if ncz.
        if (strstr(ptr->hfs0_secure.string_table[loc].name, ".ncz"))
        {
            ncz_start_install(ptr->hfs0_secure.string_table[loc].name, ptr->hfs0_secure.file_table[loc].data_size, ptr->hfs0_secure.raw_data_offset + ptr->hfs0_secure.file_table[loc].data_offset, storage_id, mode, ptr->f);
        }

        else
        {
            if (R_FAILED(nca_start_install(ptr->hfs0_secure.string_table[loc].name, ptr->hfs0_secure.raw_data_offset + ptr->hfs0_secure.file_table[loc].data_offset, storage_id, mode, ptr->f, NULL)))
            {
                print_message_loop_lock("\nfailed to install %s\n", ptr->hfs0_secure.string_table[loc].name);
                break;
            }
        }
    }

    free(cnmt_struct.cnmt_infos);
}

bool xci_get_partition(hfs0_structs_t *hfs0, u64 offset, InstallProtocalMode mode)
{
    hfs0_get_header(hfs0, offset, mode);
    if (!hfs0_check_if_magic_valid(hfs0->header.magic))
        return false;
    hfs0_populate_table_size_offsets(hfs0, offset);
    hfs0_populate_file_table(hfs0, mode);
    hfs0_populate_string_table(hfs0, mode);
    hfs0->raw_data_size = hfs0_get_total_raw_data_size(hfs0);
    return true;
}

void xci_setup_install(const char *file_name, NcmStorageId storage_id, InstallProtocalMode mode)
{
    // create main struct on the stack which gets passed around the whole program.
    xci_structs_t ptr;

    if (mode == SD_CARD_INSTALL)
        if (!(ptr.f = fopen(file_name, "rb")))
            return;

    // have all structs point to the same file.
    ptr.hfs0_secure.f = ptr.hfs0_normal.f = ptr.hfs0_update.f = ptr.hfs0_root.f = ptr.f;

    // fill the root hfs0 partition.
    if (!xci_get_partition(&ptr.hfs0_root, HFS0_HEADER_OFFSET, mode))
        return;

    // find the secure partition and fill that in also.
    int loc = hfs0_search_string_table(&ptr.hfs0_root, XCI_SECURE);
    if (loc == -1)
        return;

    if (!xci_get_partition(&ptr.hfs0_secure, ptr.hfs0_root.raw_data_offset + ptr.hfs0_root.file_table[loc].data_offset, mode))
        return;

    xci_start_install(&ptr, storage_id, mode);
    hfs0_free_structs(&ptr.hfs0_root);
    hfs0_free_structs(&ptr.hfs0_secure);
}