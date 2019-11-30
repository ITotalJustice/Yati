#include <stdio.h>
#include <switch.h>

#include "ncm.h"
#include "util.h"


Result ncm_open_storage(NcmContentStorage *ncm_storage, NcmStorageId storage_id)
{
    Result rc = ncmOpenContentStorage(ncm_storage, storage_id);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open content storage\n");
    return rc;
}

bool ncm_check_if_placeholder_exists(NcmContentStorage *ncm_storage, NcmPlaceHolderId *ncm_placeholder_id)
{
    bool exists_out = 0;
    if (R_FAILED(ncmContentStorageHasPlaceHolder(ncm_storage, &exists_out, ncm_placeholder_id)))
        print_message_loop_lock("failed to check if placeholder exists\n");
    return exists_out;
}

Result ncm_generate_placeholder_id(NcmContentStorage *ncm_storage, NcmPlaceHolderId *placeholder_out)
{
    Result rc = ncmContentStorageGeneratePlaceHolderId(ncm_storage, placeholder_out);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to generate placeholder ID\n");
    return rc;
}

Result ncm_create_placeholder(NcmContentStorage *ncm_storage, NcmContentId *ncm_content_id, NcmPlaceHolderId *ncm_placeholder_id, u64 size)
{
    Result rc = ncmContentStorageCreatePlaceHolder(ncm_storage, ncm_content_id, ncm_placeholder_id, size);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to create placeholder\n");
    return rc;
}

Result ncm_get_placeholder_path(NcmContentStorage *ncm_storage, NcmPlaceHolderId *ncm_placeholder_id, char *out, size_t size)
{
    Result rc = ncmContentStorageGetPlaceHolderPath(ncm_storage, out, size, ncm_placeholder_id);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to get placeholder path\n");
    return rc;
}

Result ncm_write_placeholder(NcmContentStorage *ncm_storage, NcmPlaceHolderId *ncm_placeholder_id, u64 *offset, void *buf, size_t buf_size)
{
    Result rc = ncmContentStorageWritePlaceHolder(ncm_storage, ncm_placeholder_id, *offset, buf, buf_size);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to write to placeholder\n");
    *offset += buf_size;
    return rc;
}

Result ncm_delete_placeholder(NcmContentStorage *ncm_storage, NcmPlaceHolderId *ncm_placeholder_id)
{
    Result rc = ncmContentStorageDeletePlaceHolder(ncm_storage, ncm_placeholder_id);
    if (R_FAILED(rc))
        printf("failed to delete placeholder\n");
    return rc;
}

void ncm_delete_all_placeholders(void)
{
    NcmContentStorage sd_storage;
    if (R_SUCCEEDED(ncm_open_storage(&sd_storage, NcmStorageId_SdCard)))
    {
        if (R_FAILED(ncmContentStorageCleanupAllPlaceHolder(&sd_storage)))
            printf("failed to delete all placeholder from sd card\n");
        ncm_close_storage(&sd_storage);
    }
    NcmContentStorage nand_storage;
    if (R_SUCCEEDED(ncm_open_storage(&nand_storage, NcmStorageId_BuiltInUser)))
    {
        if (R_FAILED(ncmContentStorageCleanupAllPlaceHolder(&nand_storage)))
            printf("failed to delete all placeholder from nand\n");
        ncm_close_storage(&nand_storage);
    }
}

Result ncm_register_placeholder(NcmContentStorage *ncm_storage, NcmContentId *ncm_content_id, NcmPlaceHolderId *ncm_placeholder_id)
{
    Result rc = ncmContentStorageRegister(ncm_storage, ncm_content_id, ncm_placeholder_id);
    if (R_FAILED(rc))
        printf("failed to register placeholder\n");
    return rc;
}

bool ncm_check_if_nca_exists(NcmContentStorage *ncm_storage, NcmContentId *ncm_content_id)
{
    bool exist_out = 0;
    Result rc = ncmContentStorageHas(ncm_storage, &exist_out, ncm_content_id);
    if (R_FAILED(rc))
        printf("failed to check if nca exists\n");
    return exist_out;
}

Result ncm_delete_nca(NcmContentStorage *ncm_storage, NcmContentId *ncm_content_id)
{
    Result rc = ncmContentStorageDelete(ncm_storage, ncm_content_id);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to delete nca\n");
    return rc;
}

Result ncm_get_storage_path(NcmContentStorage *ncm_storage, char *path_out, NcmContentId *ncm_content_id)
{
    Result rc = ncmContentStorageGetPath(ncm_storage, path_out, FS_MAX_PATH, ncm_content_id);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to get storage path\n");
    return rc;
}

s64 ncm_get_placeholder_size(NcmContentStorage *ncm_storage, NcmPlaceHolderId *placeholder_id)
{
    s64 size = 0;
    Result rc = ncmContentStorageGetSizeFromPlaceHolderId(ncm_storage, &size, placeholder_id);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to get size of placeholder\n");
    return size;
}

s64 ncm_get_nca_size(NcmContentStorage *ncm_storage, NcmContentId *ncm_content_id)
{
    s64 size = 0;
    Result rc = ncmContentStorageGetSizeFromContentId(ncm_storage, &size, ncm_content_id);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to get size of nca\n");
    return size;
}

Result ncm_read_nca_file(NcmContentStorage *ncm_storage, void *out, size_t data_size, u64 offset, NcmContentId *ncm_content_id)
{
    Result rc = ncmContentStorageReadContentIdFile(ncm_storage, out, data_size, ncm_content_id, offset);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to read data from the nca\n");
    return rc;
}

void ncm_close_storage(NcmContentStorage *ncm_storage)
{
    ncmContentStorageClose(ncm_storage);
    serviceClose(&ncm_storage->s);
}

Result ncm_open_database(NcmContentMetaDatabase *db, NcmStorageId storage_id)
{
    Result rc = ncmOpenContentMetaDatabase(db, storage_id);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open database\n");
    return rc;
}

Result ncm_set_database(NcmContentMetaDatabase *db, NcmContentMetaKey *key, void *data, size_t data_size)
{
    Result rc = ncmContentMetaDatabaseSet(db, key, (NcmContentMetaHeader*)data, data_size);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to set meta database\n");
    return rc;
}

Result ncm_commit_database(NcmContentMetaDatabase *db)
{
    Result rc = ncmContentMetaDatabaseCommit(db);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to commit database\n");
    return rc;
}

void ncm_close_database(NcmContentMetaDatabase *db)
{
    ncmContentMetaDatabaseClose(db);
    serviceClose(&db->s);
}

s64 ncm_get_storage_free_space(NcmStorageId storage_id)
{
    s64 size = 0;
    NcmContentStorage ncm_storage;
    if (R_FAILED(ncm_open_storage(&ncm_storage, storage_id)))
        return size;
    if (R_FAILED(ncmContentStorageGetFreeSpaceSize(&ncm_storage, &size)))
        printf("failed to get free storage space\n");
    ncm_close_storage(&ncm_storage);
    return size;
}

s64 ncm_get_storage_total_size(NcmStorageId storage_id)
{
    s64 size = 0;
    NcmContentStorage ncm_storage;
    if (R_FAILED(ncm_open_storage(&ncm_storage, storage_id)))
        return size;
    if (R_FAILED(ncmContentStorageGetTotalSpaceSize(&ncm_storage, &size)))
        printf("failed to get total storage space\n");
    ncm_close_storage(&ncm_storage);
    return size;
}

u64 ncm_get_app_id_from_title_id(u64 title_id, NcmContentMetaType contentMetaType)
{
    switch (contentMetaType)
    {
        case NcmContentMetaType_Application:
            return title_id;
        case NcmContentMetaType_Patch:
            return title_id ^ 0x800;
        case NcmContentMetaType_AddOnContent:
            return title_id ^ 0x1000;
        default:
            print_message_loop_lock("incorrect content meta type %u\n", contentMetaType);
            return title_id;                              
    }
}