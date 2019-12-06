#ifndef _FS_H_
#define _FS_H_

#include <switch.h>


typedef struct
{
    u8 version;                 // always 1.
    u8 _0x01;                   // padding.
    u64 permissions_bitmask;
    u32 data_size;
    u32 content_owner_id_section_size;

} fs_access_header_t;

typedef struct
{
    u8 version;                 // always 1.
    u8 _0x01;                   // padding.
    u64 permissions_bitmask;
    u8 _0xC[0x20];
} fs_access_control_t;


/*
*   FS FILE
*/

// open a file.
Result fs_open_file(FsFileSystem *fs_system, u32 mode, FsFile *file, const char *path, ...);

// create a file.
Result fs_create_file(FsFileSystem *fs_system, const char *path, s64 size, u32 option);

// delete a file.
Result fs_delete_file(FsFileSystem *fs_system, const char *path);

//
Result fs_rename_file(FsFileSystem *system, const char *old, const char *new);

// get the size of a file.
s64 fs_get_file_size(FsFile *file);

//
Result fs_set_file_size(FsFile *file, s64 size);

// same as fread().
size_t fs_read_file(void *out, u64 size, s64 offset, u32 option, FsFile *file);

//
Result fs_write_file(FsFile *file, u64 offset, void *out, u64 size, u32 option);

//
Result fs_flush_file(FsFile *file);

// close file.
void fs_close_file(FsFile *file);


/*
*   FS DIR
*/

// open a dir.
Result fs_open_dir(FsFileSystem *fs_system, u32 mode, FsDir *dir, const char *path, ...);

// create a dir.
Result fs_create_dir(FsFileSystem *fs_system, const char *path);

// delete a dir.
Result fs_delete_dir(FsFileSystem *fs_system, const char *path);

// delete a dir recursively.
Result fs_delete_dir_rec(FsFileSystem *fs_system, const char *path);

// similar to while((de = readir(dir)))
s64 fs_read_dir(FsDir *dir, size_t max_files, FsDirectoryEntry *out);

// get the total of entries in a dir (recursive???).
s64 fs_get_dir_total(FsDir *dir);

//
s64 fs_search_dir_for_file(FsDir *dir, const char *file);

//
bool fs_search_dir_for_file_2(FsDir *dir, FsDirectoryEntry *out, const char *file);

//
bool fs_get_file_in_dir_and_open(FsFileSystem *system, FsDir *dir, FsFile *out, const char *file, uint32_t mode);

// close dir.
void fs_close_dir(FsDir *dir);


/*
*   FS SYSTEM
*/

// open file system.
Result fs_open_system(FsFileSystem *fs_system, FsFileSystemType fs_type, const char *path, ...);

//
Result fs_open_system_with_ID(FsFileSystem *system, u64 title_ID, FsFileSystemType fs_type, const char *path);

// open file system with ID.
Result fs_open_system_with_patch(FsFileSystem *fs_system, u64 title_ID, FsFileSystemType fs_type);

// mount the sd card.
Result fs_mount_sd_card(FsFileSystem *fs_system);

// close file system.
void fs_close_system(FsFileSystem *fs_system);


/*
*   FS STORAGE
*/

//
Result fs_open_stoarge_by_current_process(FsStorage *out);

//
Result fs_open_stoarge_by_id(FsStorage *out, u64 data_id, NcmStorageId storage_id);

//
Result fs_read_storage(FsStorage *storage, void *out, u64 size, s64 offset);

//
Result fs_write_stoarge(FsStorage *storage, const void *in, u64 size, s64 offset);

//
Result fs_flush_storage(FsStorage *storage);

//
s64 fs_get_storage_size(FsStorage *storage);

//
Result fs_set_storage_size(FsStorage *storage, s64 size);

//
void fs_close_storage(FsStorage *storage);


/*
*   FS DEVICE OPERATOR
*/

//
Result fs_open_device_operator(FsDeviceOperator *out);

//
bool fs_is_sd_card_inserted(FsDeviceOperator *d);

//
bool fs_is_game_card_inserted(FsDeviceOperator *d);

//
Result fs_get_game_card_handle(FsDeviceOperator *d, FsGameCardHandle *out);

//
Result fs_open_game_card(const FsGameCardHandle *handle, FsGameCardPartition partition, FsFileSystem *out);

//
u_int8_t fs_get_game_card_attribute(FsDeviceOperator *d, const FsGameCardHandle *handle);

//
void fs_close_device_operator(FsDeviceOperator *d);


/*
*   FS MISC
*/

// set the archive bit for a folder, to be used with split nsp's.
Result fs_set_archive_bit(const char *path, ...);

// get free size of the sd card.
s64 fs_get_sd_free_space(void);

// get free size of the nand.
s64 fs_get_nand_free_space(void);

// get the total size of the selected storage device (sd card / nand).
s64 fs_get_free_space_from_path(FsFileSystem* fs, const char* path, ...);

// check if exfat is supported.
// returns true if true.
bool fs_is_exfat_supported(void);

// returns the app_id from the given rights_id.
u64 fs_get_app_id_from_rights_id(FsRightsId rights_id);

// returns the key_gen from the given rights_id.
u64 fs_get_key_gen_from_rights_id(FsRightsId rights_id);


/*
*   IPC functions
*/

//
Result fs_open_game_card_storage(const FsGameCardHandle *handle, FsStorage *out, FsGameCardPartition partition);

#endif