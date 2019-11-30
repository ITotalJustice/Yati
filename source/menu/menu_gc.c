#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "fs.h"
#include "cnmt.h"
#include "nca.h"
#include "util.h"



// this was copy / pasted from my gc_dump tool.
// will clean this up and make each significant section a function call.

int gc_menu_start(NcmStorageId storage_id)
{
    FsDeviceOperator d_op;
    memset(&d_op, 0, sizeof(FsDeviceOperator));
    if (R_SUCCEEDED(fs_open_device_operator(&d_op)))
    {
        if (fs_is_game_card_inserted(&d_op))
        {
            FsGameCardHandle gc_handle;
            memset(&gc_handle, 0, sizeof(FsGameCardHandle));

            if (R_SUCCEEDED(fs_get_game_card_handle(&d_op, &gc_handle)))
            {
                FsFileSystem system;
                memset(&system, 0, sizeof(FsFileSystem));
                if (R_SUCCEEDED(fs_open_game_card(&gc_handle, FsGameCardPartition_Secure, &system)))
                {
                    FsDir gc_dir;
                    memset(&gc_dir, 0, sizeof(FsDir));
                    if (R_SUCCEEDED(fs_open_dir(&system, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &gc_dir, "/")))
                    {
                        FsDirectoryEntry entry;
                        memset(&entry, 0, sizeof(FsDirectoryEntry));
                        if (fs_search_dir_for_file_2(&gc_dir, &entry, "cnmt.nca"))
                        {
                            FsFile cnmt_file;
                            memset(&cnmt_file, 0, sizeof(FsFile));
                            if (R_SUCCEEDED(fs_open_file(&system, FsOpenMode_Read, &cnmt_file, "/%s", entry.name)))
                            {
                                if (R_SUCCEEDED(nca_start_install(entry.name, 0, storage_id, GC_INSTALL, NULL, &cnmt_file)))
                                {
                                    fs_close_file(&cnmt_file);
                                    fs_close_dir(&gc_dir);

                                    cnmt_struct_t cnmt_struct;
                                    memset(&cnmt_struct, 0, sizeof(cnmt_struct_t));
                                    strcpy(cnmt_struct.cnmt_name, entry.name);
                                    cnmt_struct.storage_id = storage_id;

                                    if (R_SUCCEEDED(cnmt_open(&cnmt_struct)))
                                    {
                                        FsDir n_dir;
                                        fs_open_dir(&system, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &n_dir, "/");

                                        for (u_int32_t i = 1; i < cnmt_struct.total_cnmt_infos; i++)
                                        {
                                            char new_nca_name[FS_MAX_PATH] = {0};
                                            nca_get_string_from_id(cnmt_struct.cnmt_infos[i].content_id, new_nca_name);
                                            
                                            FsDirectoryEntry nca_entry;
                                            if (!fs_search_dir_for_file_2(&n_dir, &nca_entry, new_nca_name))
                                                break;

                                            FsFile nca_file;
                                            if (R_FAILED(fs_open_file(&system, FsOpenMode_Read, &nca_file, "/%s", nca_entry.name)))
                                                break;

                                            if (R_FAILED(nca_start_install(new_nca_name, 0, storage_id, GC_INSTALL, NULL, &nca_file)))
                                            {
                                                print_message_loop_lock("\nfailed to install %s\n", nca_entry.name);
                                                break;
                                            }

                                            fs_close_file(&nca_file);
                                        }
                                        fs_close_dir(&n_dir);
                                        free(cnmt_struct.cnmt_infos);
                                    }
                                }
                            }
                        }
                        fs_close_dir(&gc_dir);
                    }
                    fs_close_system(&system);
                }
            }
        }
        fs_close_device_operator(&d_op);
    }

    return 0;
}