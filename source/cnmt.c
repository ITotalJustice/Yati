#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "fs.h"
#include "ncm.h"
#include "ns.h"
#include "cnmt.h"
#include "util.h"


bool cnmt_set_content_storage_record(cnmt_ptr_t *ptr)
{
    uint64_t app_id = ncm_get_app_id_from_title_id(ptr->key.title_id, ptr->key.meta_type);
    content_storage_record_t cnmt_storage_record = { ptr->key, ptr->storage_id };

    uint8_t *app_record = malloc(sizeof(content_storage_record_t));
    size_t app_record_size = 0;

    // check if any app_cnmt data exists, if so, add it to the start of the app_record buffer.
    uint32_t cnmt_count = ns_count_application_content_meta(app_id);
    if (cnmt_count)
    {
        app_record_size = cnmt_count * sizeof(content_storage_record_t);
        uint8_t *old_app_buf = malloc(app_record_size);
        memset(old_app_buf, 0, app_record_size);

        ns_list_application_record_content_meta(0, app_id, old_app_buf, app_record_size, cnmt_count);

        app_record = realloc(app_record, app_record_size + sizeof(content_storage_record_t));
        memcpy(app_record, old_app_buf, app_record_size);
        free(old_app_buf);
    }

    // push record.
    memcpy(&app_record[app_record_size], &cnmt_storage_record, sizeof(content_storage_record_t));
    ns_delete_application_record(app_id);
    Result rc = ns_push_application_record(app_id, (content_storage_record_t *)app_record, app_record_size + sizeof(content_storage_record_t));
    free(app_record);

    if (R_FAILED(rc))
        return false;
    return true;
}

bool cnmt_set_content_info(cnmt_ptr_t *ptr)
{
    Result rc = 0;
    NcmContentMetaDatabase db;
    rc = ncm_open_database(&db, ptr->storage_id);
    if (R_FAILED(rc))
        return false;

    size_t data_size = sizeof(NcmContentMetaHeader) + ptr->header.meta_header.extended_header_size + (sizeof(NcmContentInfo) * ptr->header.meta_header.content_count);
    uint8_t *data = malloc(data_size);
    uint64_t data_offset = 0;
    memcpy(&data[data_offset], &ptr->header.meta_header, sizeof(NcmContentMetaHeader));
    memcpy(&data[data_offset += sizeof(content_meta_header_t)], ptr->extended_header, ptr->header.meta_header.extended_header_size);
    memcpy(&data[data_offset += ptr->header.meta_header.extended_header_size], ptr->info, ptr->header.meta_header.content_count * sizeof(content_info_t));

    rc = ncm_set_database(&db, &ptr->key, data, data_size);
    free(data);
    if (R_FAILED(rc))
    {
        ncm_close_database(&db);
        return false;
    }

    rc = ncm_commit_database(&db);
    if (R_FAILED(rc))
    {
        ncm_close_database(&db);
        return false;
    }

    ncm_close_database(&db);
    return true;
}

bool cnmt_setup_content_info(cnmt_ptr_t *ptr, NcmContentId *content_id)
{
    ptr->info = malloc(ptr->header.meta_header.content_count + 1 * (sizeof(content_info_t)));
    *ptr->info[0].content_id.id = *content_id->c;
    *ptr->info[0].size = fs_get_file_size(&ptr->storage.file);
    ptr->info[0].content_type = NcmContentType_Meta;
    ptr->info[0].id_offset = 0;
    uint32_t info_total = 1;

    for (uint32_t i = 0; i < ptr->header.meta_header.content_meta_count; i++, ptr->offset += sizeof(NcmPackagedContentInfo))
    {
        NcmPackagedContentInfo packed_temp;
        size_t read = fs_read_file(&packed_temp, sizeof(NcmPackagedContentInfo), ptr->offset, FsReadOption_None, &ptr->storage.file);

        if (read != sizeof(NcmPackagedContentInfo))
            return false;

        // skip deltas...
        if (packed_temp.info.content_type == NcmContentType_DeltaFragment)
            continue;
        
        // if not delta, copy data into struct.
        memcpy(&ptr->info[info_total], &packed_temp.info, sizeof(NcmContentInfo));
        info_total++;
    }

    ptr->header.meta_header.content_count = info_total;
    return true;
}

void cnmt_setup_key(cnmt_ptr_t *ptr)
{
    ptr->key.title_id = ptr->header.title_id;
    ptr->key.version = ptr->header.title_version;
    ptr->key.meta_type = ptr->header.meta_type;
    ptr->key.install_type = NcmContentInstallType_Full;
    *ptr->key._0xE = 0;
}

void cnmt_set_ignore_sys_version(cnmt_ptr_t *ptr)
{
    switch (ptr->header.meta_type)
    {
        case NcmContentMetaType_Application:
            ptr->extended_header[0x8] = 0x0;
        case NcmContentMetaType_Patch:
            ptr->extended_header[0x8] = 0x0;
    }
}

bool cnmt_get_extended_header(cnmt_ptr_t *ptr)
{
    ptr->extended_header = malloc(ptr->header.meta_header.extended_header_size);
    size_t read = fs_read_file(ptr->extended_header, ptr->header.meta_header.extended_header_size, ptr->offset, FsReadOption_None, &ptr->storage.file);
    if (read != ptr->header.meta_header.extended_header_size)
        return false;
    return true;
}

bool cnmt_get_header(cnmt_ptr_t *ptr)
{
    size_t read = fs_read_file(&ptr->header, CNMT_HEADER_SIZE, 0, FsReadOption_None, &ptr->storage.file);
    if (read != CNMT_HEADER_SIZE)
        return false;
    ptr->offset = read;
    return true;
}

void cnmt_close_registered(cnmt_storage_t *ptr)
{
    fs_close_file(&ptr->file);
    fs_close_dir(&ptr->dir);
    fs_close_system(&ptr->system);
}

bool cnmt_open_registered(cnmt_storage_t *out, const char *path)
{
    if (R_SUCCEEDED(fs_open_system(&out->system, FsFileSystemType_ContentMeta, path)))
    {
        if (R_SUCCEEDED(fs_open_dir(&out->system, FsDirOpenMode_ReadFiles | FsDirOpenMode_ReadDirs, &out->dir, "/")))
        {
            if (fs_get_file_in_dir_and_open(&out->system, &out->dir, &out->file, "cnmt", FsOpenMode_Read))
            {
                return true;
            }
            fs_close_dir(&out->dir);
        }
        fs_close_system(&out->system);
    }

    return false;
}

bool cnmt_get_registered_path(char *out, uint8_t storage_id, NcmContentId *content_id)
{
    bool res = true;
    NcmContentStorage storage;
    if (R_SUCCEEDED(ncm_open_storage(&storage, storage_id)))
    {
        if (R_FAILED(ncm_get_storage_path(&storage, out, content_id)))
        {
            if (R_FAILED(ncm_get_storage_path(&storage, out, content_id)))
                res = false;
            ncm_close_storage(&storage);
        }
    }
    return res;
}

bool cnmt_start(content_ids_t *out, NcmContentId *content_id, uint8_t storage_id)
{
    bool res = 0;
    cnmt_ptr_t ptr;

    char path[256];
    if (!(res = cnmt_get_registered_path(path, storage_id, content_id)))
        return res;

    if ((res = cnmt_open_registered(&ptr.storage, path)))
    {
        if ((res = cnmt_get_header(&ptr)))
        {
            if ((res = cnmt_get_extended_header(&ptr)))
            {
                cnmt_set_ignore_sys_version(&ptr);
                cnmt_setup_key(&ptr);
                if ((res = cnmt_setup_content_info(&ptr, content_id)))
                {
                    if ((res = cnmt_set_content_info))
                    {
                        if ((res = cnmt_set_content_storage_record(&ptr)))
                        {
                            for (uint32_t i = 1, j = 0; i < ptr.header.meta_header.content_count; i++, j++)
                                memcpy(ptr.info[i].content_id.id, out->id[j].id, sizeof(NcmContentId));
                            out->total = ptr.header.meta_header.content_count - 1;
                        }
                    }
                    free(&ptr.info);
                }
                free(ptr.extended_header);
            }
        }
        cnmt_close_registered(&ptr.storage);
    }
    return res;
}