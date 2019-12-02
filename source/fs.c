#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <switch.h>

#include "util.h"


/*
*   FS FILE
*/

Result fs_open_file(FsFileSystem *system, u32 mode, FsFile *file, const char *path, ...)
{
    char new_path[FS_MAX_PATH];
    va_list arg;
    va_start(arg, path);
    vsprintf(new_path, path, arg);
    va_end(arg);

    Result rc = fsFsOpenFile(system, new_path, mode, file);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open file %s\n", new_path);
    return rc;
}

Result fs_create_file(FsFileSystem *system, const char *path, s64 size, u32 option)
{
    Result rc = fsFsCreateFile(system, path, size, option);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to create file %s\n", path);
    return rc;
}

Result fs_delete_file(FsFileSystem *system, const char *path)
{
    Result rc = fsFsDeleteFile(system, path);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to delete file %s\n", path);
    return rc;
}

Result fs_rename_file(FsFileSystem *system, const char *old, const char *new)
{
    Result rc = fsFsRenameFile(system, old, new);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to rename old file %s to %s\n", old, new);
    return rc;
}

s64 fs_get_file_size(FsFile *file)
{
    s64 size = 0;
    if (R_FAILED(fsFileGetSize(file, &size)))
        print_message_loop_lock("failed to get file size\n");
    return size;
}

Result fs_set_file_size(FsFile *file, s64 size)
{
    Result rc = fsFileSetSize(file, size);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to set file size ...\n");
    return rc;
}

size_t fs_read_file(void *out, u64 size, s64 offset, u32 option, FsFile *file)
{
    size_t total = 0;
    if (R_FAILED(fsFileRead(file, offset, out, size, option, &total)))
        print_message_loop_lock("failed to read file\n");
    if (total != size)
        print_message_loop_lock("file read missmatch! total = %ld size = %ld\n", total, size);
    return total;
}

Result fs_write_file(FsFile *file, u64 offset, void *out, u64 size, u32 option)
{
    Result rc = fsFileWrite(file, offset, out, size, option);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to write to file ...\n");
    return rc;
}

Result fs_flush_file(FsFile *file)
{
    Result rc = fsFileFlush(file);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to flush file ...\n");
    return rc;
}

void fs_close_file(FsFile *file)
{
    fsFileClose(file);
    serviceClose(&file->s);
}


/*
*   FS DIR
*/

Result fs_open_dir(FsFileSystem *system, u32 mode, FsDir *dir, const char *path, ...)
{
    char new_path[FS_MAX_PATH];
    va_list arg;
    va_start(arg, path);
    vsprintf(new_path, path, arg);
    va_end(arg);

    Result rc = fsFsOpenDirectory(system, new_path, mode, dir);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open dir %s\n", new_path);
    return rc;
}

Result fs_create_dir(FsFileSystem *system, const char *path)
{
    Result rc = fsFsCreateDirectory(system, path);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to create dir %s\n", path);
    return rc;
}

Result fs_delete_dir(FsFileSystem *system, const char *path)
{
    Result rc = fsFsDeleteDirectory(system, path);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to delete dir %s\n", path);
    return rc;
}

Result fs_delete_dir_rec(FsFileSystem *system, const char *path)
{
    Result rc = fsFsDeleteDirectoryRecursively(system, path);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to delete dir recursively %s\n", path);
    return rc;
}

s64 fs_read_dir(FsDir *dir, size_t max_files, FsDirectoryEntry *out)
{
    s64 total = 0;
    if (R_FAILED(fsDirRead(dir, &total, max_files, out)))
        print_message_loop_lock("failed to read dir\n");
    if (total != max_files)
        print_message_loop_lock("number of files read missmatch! total = %ld max_files = %ld\n", total, max_files);
    return total;
}

s64 fs_get_dir_total(FsDir *dir)
{
    s64 total = 0;
    if (R_FAILED(fsDirGetEntryCount(dir, &total)))
        print_message_loop_lock("failed get total\n");
    return total;
}

s64 fs_search_dir_for_file(FsDir *dir, const char *file)
{
    for (s64 i = 0, total = fs_get_dir_total(dir); i < total; i++)
    {
        FsDirectoryEntry entry;
        fs_read_dir(dir, 1, &entry);
        if (strstr(entry.name, file))
            return i;
    }
    return -1;
}

bool fs_search_dir_for_file_2(FsDir *dir, FsDirectoryEntry *out, const char *file)
{
    for (s64 i = 0, total = fs_get_dir_total(dir); i < total; i++)
    {
        FsDirectoryEntry entry;
        fs_read_dir(dir, 1, &entry);
        if (strstr(entry.name, file))
        {
            memcpy(out, &entry, sizeof(FsDirectoryEntry));
            return true;
        }
    }
    
    print_message_loop_lock("couldn't find file %s\n", file);
    return false;
}

void fs_close_dir(FsDir *dir)
{
    fsDirClose(dir);
    serviceClose(&dir->s);
}


/*
*   FS SYSTEM
*/

Result fs_open_system(FsFileSystem *system, FsFileSystemType fs_type, const char *path, ...)
{
    Result rc = fsOpenFileSystem(system, fs_type, path);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open file system %s\n", path);
    return rc;
}

Result fs_open_system_with_ID(FsFileSystem *system, u64 title_ID, FsFileSystemType fs_type, const char *path)
{
    Result rc = fsOpenFileSystemWithId(system, title_ID, fs_type, path);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open file system with ID %s\n", path);
    return rc;
}

Result fs_open_system_with_patch(FsFileSystem *system, u64 title_ID, FsFileSystemType fs_type)
{
    Result rc = fsOpenFileSystemWithPatch(system, title_ID, fs_type);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open file system with patch %ld\n", title_ID);
    return rc;
}

Result fs_mount_sd_card(FsFileSystem *system)
{
    Result rc = fsOpenSdCardFileSystem(system);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open sd card\n");
    return rc;
}

void fs_close_system(FsFileSystem *system)
{
    fsFsClose(system);
    serviceClose(&system->s);
}


/*
*   FS STORAGE
*/

Result fs_open_stoarge_by_current_process(FsStorage *out)
{
    Result rc = fsOpenDataStorageByCurrentProcess(out);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open storage by current process\n");
    return rc;
}

Result fs_open_stoarge_by_id(FsStorage *out, u_int64_t data_id, NcmStorageId storage_id)
{
    Result rc = fsOpenDataStorageByDataId(out, data_id, storage_id);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open stoarge using data id %lu\n", data_id);
    return rc;
}

Result fs_read_storage(FsStorage *storage, void *out, u64 size, s64 offset)
{
    Result rc = fsStorageRead(storage, offset, out, size);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to read storage...\n");
    return rc;
}

Result fs_write_stoarge(FsStorage *storage, const void *in, u64 size, s64 offset)
{
    Result rc = fsStorageWrite(storage, offset, in, size);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to write to storage...\n");
    return rc;
}

Result fs_flush_storage(FsStorage *storage)
{
    Result rc = fsStorageFlush(storage);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to flush storage...\n");
    return rc;
}

s64 fs_get_storage_size(FsStorage *storage)
{
    s64 size = 0;
    if (R_FAILED(fsStorageGetSize(storage, &size)))
        print_message_loop_lock("failed to get storage size...\n");
    return size;
}

Result fs_set_storage_size(FsStorage *storage, s64 size)
{
    Result rc = fsStorageSetSize(storage, size);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to set storage size to %ld...\n", size);
    return rc;
}

void fs_close_storage(FsStorage *storage)
{
    fsStorageClose(storage);
    serviceClose(&storage->s);
}


/*
*   FS DEVICE OPERATOR
*/

Result fs_open_device_operator(FsDeviceOperator *out)
{
    Result rc = fsOpenDeviceOperator(out);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open device operator...\n");
    return rc;
}

bool fs_is_sd_card_inserted(FsDeviceOperator *d)
{
    bool inserted = false;
    if (R_FAILED(fsDeviceOperatorIsSdCardInserted(d, &inserted)))
        print_message_loop_lock("failed to check if sd card is inserted...\n");
    return inserted;
}

bool fs_is_game_card_inserted(FsDeviceOperator *d)
{
    bool inserted = false;
    if (R_FAILED(fsDeviceOperatorIsGameCardInserted(d, &inserted)))
        print_message_loop_lock("failed to check if sd card is inserted...\n");
    return inserted;
}

Result fs_get_game_card_handle(FsDeviceOperator *d, FsGameCardHandle *out)
{
    Result rc = fsDeviceOperatorGetGameCardHandle(d, out);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to get gamecard handle...\n");
    return rc;
}

Result fs_open_game_card(const FsGameCardHandle *handle, FsGameCardPartition partition, FsFileSystem *out)
{
    Result rc = fsOpenGameCardFileSystem(out, handle, partition);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to open gamecard...\n");
    return rc;
}

u_int8_t fs_get_game_card_attribute(FsDeviceOperator *d, const FsGameCardHandle *handle)
{
    u_int8_t att = 0;
    if (R_FAILED(fsDeviceOperatorGetGameCardAttribute(d, handle, &att)))
        print_message_loop_lock("failed to get game card attribute...\n");
    return att;
}

void fs_close_device_operator(FsDeviceOperator *d)
{
    fsDeviceOperatorClose(d);
}


/*
*   FS MISC
*/

Result fs_set_archive_bit(const char *path, ...)
{
    char new_path[FS_MAX_PATH];
    va_list arg;
    va_start(arg, path);
    vsprintf(new_path, path, arg);
    va_end(arg);

    Result rc = 0;
    FsFileSystem fs;
    if (R_FAILED(rc = fs_mount_sd_card(&fs)))
        return rc;
    if (R_FAILED(rc = fsFsSetConcatenationFileAttribute(&fs, new_path)))
        print_message_loop_lock("failed to set archive bit for %s\n", new_path);
    fs_close_system(&fs);
    return rc;
}

s64 fs_get_sd_free_space()
{
    Result rc = 0;
    s64 size = 0;
    FsFileSystem fs;
    if (R_FAILED(rc = fs_mount_sd_card(&fs)))
        return size;
    if (R_FAILED(fsFsGetFreeSpace(&fs, "/", &size)))
        print_message_loop_lock("failed to get sd card free space\n");
    fs_close_system(&fs);
    return size;
}

s64 fs_get_nand_free_space()
{
    Result rc = 0;
    s64 size = 0;
    FsFileSystem fs;
    if (R_FAILED(rc = fs_mount_sd_card(&fs)))
        return size;
    if (R_FAILED(fsFsGetFreeSpace(&fs, "/", &size)))
        print_message_loop_lock("failed to get nand free space\n");
    fs_close_system(&fs);
    return size;
}

s64 fs_get_free_space_from_path(FsFileSystem* fs, const char* path, ...)
{
    char new_path[FS_MAX_PATH];
    va_list arg;
    va_start(arg, path);
    vsprintf(new_path, path, arg);
    va_end(arg);

    s64 size = 0;
    if (R_FAILED(fsFsGetTotalSpace(fs, new_path, &size)))
        print_message_loop_lock("failed to get something size\n");
    return size;
}

bool fs_is_exfat_supported(void)
{
    bool supported = 0;
    if (R_FAILED(fsIsExFatSupported(&supported)))
        print_message_loop_lock("failed to check if exfat is supported...\n");
    return supported;
}

u64 fs_get_app_id_from_rights_id(FsRightsId rights_id)
{
    return __bswap64(*(u64 *)rights_id.c);
}

u64 fs_get_key_gen_from_rights_id(FsRightsId rights_id)
{
    return __bswap64(*(u64 *)(rights_id.c + 0x8));
}


/*
*   IPC functions
*/

