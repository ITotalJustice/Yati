#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <switch.h>


Result fs_create_file(FsFileSystem *fs_system, const char *path, u64 size, u32 option)
{
    Result rc = fsFsCreateFile(fs_system, path, size, option);
    if (R_FAILED(rc))
        printf("failed to create file %s\n", path);
    return rc;
}

Result fs_create_dir(FsFileSystem *fs_system, const char *path)
{
    Result rc = fsFsCreateDirectory(fs_system, path);
    if (R_FAILED(rc))
        printf("failed to create dir %s\n", path);
    return rc;
}

Result fs_delete_file(FsFileSystem *fs_system, const char *path)
{
    Result rc = fsFsDeleteFile(fs_system, path);
    if (R_FAILED(rc))
        printf("failed to delete file %s\n", path);
    return rc;
}

Result fs_delete_dir(FsFileSystem *fs_system, const char *path)
{
    Result rc = fsFsDeleteDirectory(fs_system, path);
    if (R_FAILED(rc))
        printf("failed to delete dir %s\n", path);
    return rc;
}

Result fs_delete_dir_rec(FsFileSystem *fs_system, const char *path)
{
    Result rc = fsFsDeleteDirectoryRecursively(fs_system, path);
    if (R_FAILED(rc))
        printf("failed to delete dir recursively %s\n", path);
    return rc;
}

Result fs_open_file(FsFileSystem *fs_system, u32 mode, FsFile *file, const char *path, ...)
{
    char new_path[FS_MAX_PATH];
    va_list arg;
    va_start(arg, path);
    vsprintf(new_path, path, arg);
    va_end(arg);

    Result rc = fsFsOpenFile(fs_system, new_path, mode, file);
    if (R_FAILED(rc))
        printf("failed to open file %s\n", new_path);
    return rc;
}

Result fs_open_dir(FsFileSystem *fs_system, u32 mode, FsDir *dir, const char *path, ...)
{
    char new_path[FS_MAX_PATH];
    va_list arg;
    va_start(arg, path);
    vsprintf(new_path, path, arg);
    va_end(arg);

    Result rc = fsFsOpenDirectory(fs_system, new_path, mode, dir);
    if (R_FAILED(rc))
        printf("failed to open dir %s\n", new_path);
    return rc;
}

Result fs_mount_sd_card(FsFileSystem *fs_system)
{
    Result rc = fsOpenSdCardFileSystem(fs_system);
    if (R_FAILED(rc))
        printf("failed to open sd card\n");
    return rc;
}

Result fs_open_system(FsFileSystem *fs_system, FsFileSystemType fs_type, const char *path)
{
    Result rc = fsOpenFileSystem(fs_system, fs_type, path);
    if (R_FAILED(rc))
        printf("failed to open file system %s\n", path);
    return rc;
}

Result fs_open_system_with_ID(FsFileSystem *fs_system, u64 title_ID, FsFileSystemType fs_type, const char *path)
{
    Result rc = fsOpenFileSystemWithId(fs_system, title_ID, fs_type, path);
    if (R_FAILED(rc))
        printf("failed to open file system with ID %s\n", path);
    return rc;
}

Result fs_open_system_with_patch(FsFileSystem *fs_system, u64 title_ID, FsFileSystemType fs_type)
{
    Result rc = fsOpenFileSystemWithPatch(fs_system, title_ID, fs_type);
    if (R_FAILED(rc))
        printf("failed to open file system with patch %ld\n", title_ID);
    return rc;
}

void fs_close_file(FsFile *file)
{
    fsFileClose(file);
    serviceClose(&file->s);
}

void fs_close_dir(FsDir *dir)
{
    fsDirClose(dir);
    serviceClose(&dir->s);
}

void fs_close_storage_ID()
{
    //fsStorageClose()
}

void fs_close_system(FsFileSystem *fs_system)
{
    fsFsClose(fs_system);
    serviceClose(&fs_system->s);
}

size_t fs_read_file(FsFile *file, u64 offset, void *buf, u64 size, u32 option)
{
    size_t total = 0;
    if (R_FAILED(fsFileRead(file, offset, buf, size, option, &total)))
        printf("failed to read file\n");
    if (total != size)
        printf("file read missmatch! total = %ld size = %ld\n", total, size);
    return total;
}

size_t fs_read_dir(FsDir *dir, u64 inval, size_t max_files, FsDirectoryEntry *buf)
{
    size_t total = 0;
    if (R_FAILED(fsDirRead(dir, inval, &total, max_files, buf)))
        printf("failed to read dir\n");
    if (total != max_files)
        printf("number of files read missmatch!\ntotal = %ld max_files = %ld", total, max_files);
    return total;
}

size_t fs_read_storage(u64 offset, void *buf, u64 size)
{
    //fsStorageRead(&fs_)
    return size;
}

size_t fs_get_dir_total(FsDir *dir)
{
    size_t total = 0;
    if (R_FAILED(fsDirGetEntryCount(dir, &total)))
        printf("failed get total\n");
    return total;
}

size_t fs_get_file_size(FsFile *file)
{
    size_t size = 0;
    if (R_FAILED(fsFileGetSize(file, &size)))
        printf("failed to get file size\n");
    return size;
}

size_t fs_get_sd_free_space()
{
    FsFileSystem fs;
    fs_mount_sd_card(&fs);
    size_t size = 0;
    if (R_FAILED(fsFsGetFreeSpace(&fs, "/", &size)))
        printf("failed to get something size\n");
    fs_close_system(&fs);
    return size;
}

size_t fs_get_nand_free_space()
{
    FsFileSystem fs;
    fs_mount_sd_card(&fs);
    size_t size = 0;
    if (R_FAILED(fsFsGetFreeSpace(&fs, "/", &size)))
        printf("failed to get something size\n");
    fs_close_system(&fs);
    return size;
}

size_t fs_get_storage_size(FsFileSystem* fs, const char* path)
{
    size_t size = 0;
    if (R_FAILED(fsFsGetTotalSpace(fs, path, &size)))
        printf("failed to get something size\n");
    return size;
}

Result fs_set_archive_bit(const char *path)
{
    char absolute_path[FS_MAX_PATH];
    snprintf(absolute_path, FS_MAX_PATH, "/switch/%s/", path);

    FsFileSystem fs;
    fs_mount_sd_card(&fs);
    Result rc = fsFsSetArchiveBit(&fs, absolute_path);
    if (R_FAILED(rc))
        printf("failed to set archive bit for %s\n", absolute_path);
    fs_close_system(&fs);
    return rc;
}