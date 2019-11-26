#ifndef _FS_H_
#define _FS_H_

#include <switch.h>

typedef enum
{
    SD_CARD = 0,
    USB_HDD = 1,
} fs_protocal;


// create a file.
Result fs_create_file(FsFileSystem *fs_system, const char *path, u64 size, u32 option);

// create a dir.
Result fs_create_dir(FsFileSystem *fs_system, const char *path);

// delete a file.
Result fs_delete_file(FsFileSystem *fs_system, const char *path);

// delete a dir.
Result fs_delete_dir(FsFileSystem *fs_system, const char *path);

// delete a dir recursively.
Result fs_delete_dir_rec(FsFileSystem *fs_system, const char *path);

// open a file.
Result fs_open_file(FsFileSystem *fs_system, u32 mode, FsFile *file, const char *path, ...);

// open a dir.
Result fs_open_dir(FsFileSystem *fs_system, u32 mode, FsDir *dir, const char *path, ...);

// mount the sd card.
Result fs_mount_sd_card(FsFileSystem *fs_system);

// open file system.
Result fs_open_system(FsFileSystem *fs_system, FsFileSystemType fs_type, const char *path);

// open file system with ID.
Result fs_open_system_with_patch(FsFileSystem *fs_system, u64 title_ID, FsFileSystemType fs_type);

// close file.
void fs_close_file(FsFile *file);

// close dir.
void fs_close_dir(FsDir *dir);

//
void fs_close_storage_ID(void);

// close file system.
void fs_close_system(FsFileSystem *fs_system);

// same as fread().
size_t fs_read_file(FsFile *file, u64 offset, void *buf, u64 size, u32 option);

// similar to while((de = readir(dir)))
size_t fs_read_dir(FsDir *dir, u64 inval, size_t max_files, FsDirectoryEntry *buf);

//
size_t fs_read_storage(u64 offset, void *buf, u64 size);

// get the total of entries in a dir (recursive???).
size_t fs_get_dir_total(FsDir *dir);

// get the size of a file.
size_t fs_get_file_size(FsFile *file);

// get free size of the sd card.
size_t fs_get_sd_free_space(void);

// get free size of the nand.
size_t fs_get_nand_free_space(void);

// get the total size of the selected storage device (sd card / nand).
size_t fs_get_storage_size(FsFileSystem* fs, const char* path);

// set the archive bit for a folder, to be used with split nsp's.
Result fs_set_archive_bit(const char *path);

#endif