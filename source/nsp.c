#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "pfs0.h"
#include "nca.h"
#include "cnmt.h"
#include "ns.h"
#include "util.h"
#include "es.h"
#include "ncz.h"


void nsp_setup_tik_cert_install(pfs0_struct_ptr *ptr, NcmStorageId storage_id, InstallProtocal mode)
{
    int tik_loc = pfs0_search_string_table(ptr, ".tik");
    int cert_loc = pfs0_search_string_table(ptr, ".cert");
    if (tik_loc == -1 || cert_loc == -1)
    {
        print_message_display("failed to find tik and cert\n");
        return;
    }

    void *tik_buf = malloc(ptr->file_table[tik_loc].data_size);
    void *cert_buf = malloc(ptr->file_table[cert_loc].data_size);
    read_data_from_protocal(mode, tik_buf, ptr->file_table[tik_loc].data_size, ptr->raw_data_offset + ptr->file_table[tik_loc].data_offset, ptr->file, NULL);
    read_data_from_protocal(mode, cert_buf, ptr->file_table[cert_loc].data_size, ptr->raw_data_offset + ptr->file_table[cert_loc].data_offset, ptr->file, NULL);

    es_import_tik_and_cert(tik_buf, ptr->file_table[tik_loc].data_size, cert_buf, ptr->file_table[cert_loc].data_size);
    free(tik_buf);
    free(cert_buf);
}

void nsp_start_install(pfs0_struct_ptr *ptr, NcmStorageId storage_id, InstallProtocal mode)
{
    int loc = pfs0_search_string_table(ptr, "cnmt.nca");
    if (loc == -1)
        return;

    if (R_FAILED(nca_start_install(ptr->string_table[loc].name, ptr->raw_data_offset + ptr->file_table[loc].data_offset, storage_id, mode, ptr->file, NULL)))
    {
        print_message_loop_lock("failed to install cnmt\n");
        return;
    }
    
    // attempt to install tik and cert.
    nsp_setup_tik_cert_install(ptr, storage_id, mode);

    cnmt_struct_t cnmt_struct;
    memset(&cnmt_struct, 0, sizeof(cnmt_struct_t));

    cnmt_struct.storage_id = storage_id;
    strcpy(cnmt_struct.cnmt_name, ptr->string_table[loc].name);

    if (R_FAILED(cnmt_open(&cnmt_struct)))
    {
        print_message_loop_lock("\nfailed to open cnmt\n");
        return;
    }

    for (u_int32_t i = 1; i < cnmt_struct.total_cnmt_infos; i++)
    {
        char new_nca_name[FS_MAX_PATH] = {0};
        nca_get_string_from_id(cnmt_struct.cnmt_infos[i].content_id, new_nca_name);

        loc = pfs0_search_string_table(ptr, new_nca_name);
        if (loc == -1)
            break;

        // check if ncz.
        if (strstr(ptr->string_table[loc].name, ".ncz"))
        {
            ncz_start_install(ptr->string_table[loc].name, ptr->file_table[loc].data_size, ptr->raw_data_offset + ptr->file_table[loc].data_offset, storage_id, mode, ptr->file);
        }

        else
        {
            if (R_FAILED(nca_start_install(ptr->string_table[loc].name, ptr->raw_data_offset + ptr->file_table[loc].data_offset, storage_id, mode, ptr->file, NULL)))
            {
                print_message_loop_lock("\nfailed to install %s\n", ptr->string_table[loc].name);
                break;
            }
        }
    }

    free(cnmt_struct.cnmt_infos);
}

void nsp_setup_install(const char *file_name, NcmStorageId storage_id, InstallProtocal mode)
{
    // create struct ptr that will point to all other structs (and .nsp file).
    pfs0_struct_ptr ptr;

    // if the protcal is sd card install.
    // open the nsp, save pointer to struct.
    if (mode == SD_CARD_INSTALL)
        if (!(ptr.file = fopen(file_name, "rb")))
            return;

    // get the header and check if the magic is valid.
    pfs0_get_header(&ptr, mode);
    if (!pfs0_check_valid_magic(ptr.header.magic))
        return;

    // fill out the tables.
    pfs0_populate_table_size_offsets(&ptr);
    pfs0_populate_file_table(&ptr, mode);
    pfs0_populate_string_table(&ptr, mode);
    ptr.raw_data_size = pfs0_get_total_raw_data_size(&ptr);

    // check if we have enough space for the selected game.
    // wont really work for ncz sadly because it'll be compressed size.
    // why doesnt the pfs0 have a u64 for total size fml.
    // if not, cleanup and exit.

    // now that we've filled out the tables, we can start installing!
    nsp_start_install(&ptr, storage_id, mode);

    // free structs that were used.
    pfs0_free_structs(&ptr);
}