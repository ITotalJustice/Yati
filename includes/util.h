#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdint.h>
#include <dirent.h>
#include <switch/services/fs.h>

#define TEMP_FILE   "temp"
#define LIST_MAX    20 // for use with ui listing.

typedef enum
{
    InstallProtocalMode_SD      = 0x0,
    InstallProtocalMode_USB     = 0x1,
    InstallProtocalMode_NTWRK   = 0x2,
    InstallProtocalMode_GC      = 0x3
} InstallProtocalMode;

typedef enum
{
    DataSize_1kb = 0x400,
    DataSize_1MiB = 0x100000,
    DataSize_2MiB = 0x200000,
    DataSize_3MiB = 0x300000,
    DataSize_4MiB = 0x400000,
    DataSize_5MiB = 0x500000,
    DataSize_6MiB = 0x600000,
    DataSize_7MiB = 0x700000,
    DataSize_8MiB = 0x800000,
    DataSize_16MiB = 0x1000000
} DataSize;

typedef struct
{
    uint8_t mode;   // see InstallProtocal.
    FILE *std_file;
    FsFile fs_file;
} install_protocal_t; // name not set.

/*
*   CURSOR FUNCTIONS.
*/

// move the cursor up / down.
uint32_t move_cursor_up(uint32_t cursor, uint32_t cursor_max);
uint32_t move_cursor_down(uint32_t cursor, uint32_t cursor_max);

// move list up / down.
uint32_t list_move_up(uint32_t list_move, uint32_t cursor, uint32_t number_of_files, uint32_t list_max);
uint32_t list_move_down(uint32_t list_move, uint32_t cursor, uint32_t list_max);


/*
*   GENERAL DIR FUNCTIONS.
*/

// open dir. return NULL if error.
DIR *open_dir(const char *directory);

// check if the path is a dir.
bool is_dir(const char *folder_to_check);

// check if a dir exists, same code as is_dir.
bool check_if_dir_exists(const char *directory);

// change dir.
// creates a dir if one doesnt exist.
bool change_dir(const char *path);

// return total files in a dir.
size_t scan_dir(const char *directory);

// return total files in a dir recursively.
size_t scan_dir_recursive(const char *directory);

// return size of folder.
size_t get_foldersize(const char *directory);

// create dir.
bool create_dir(const char *dir);

// delete dir.
void delete_dir(const char *directory);

// copy folder and all of its contents.
void copy_folder(const char *src, char *dest);

// move folder and all of its contents.
void move_folder(const char *src, char *dest);


/*
*   GENERAL FILE FUNCTIONS.
*/

// open file. return NULL if error.
FILE *open_file(const char *file, const char *mode);

// return the ext of a file.
const char *get_filename_ext(const char *filename);

// check if file exists.
bool check_if_file_exists(const char *file);

// get the size of a file.
size_t get_filesize(const char *file);

// create an empty file.
// returns 1 on success or if the file already existed.
bool create_file(const char *file);

// remove file.
// returns 1 on success or if the file didn't exist.
bool delete_file(const char *file);

// copy file.
void copy_file(const char *src, char *dest);

// move file (rename).
void move_file(const char *src, char *dest);

//
bool parse_search_from_file(const char *file, const char *filter, char* out_string);


/*
*   MESSAGE DISPLAY FUNCTIONS.
*/

// spawn swkbd.
// optional: pass in args to the clipboard.
void keyboard(char *buffer, const char *clipboard, ...);

// print message and refresh the screen after.
void print_message_display(const char *message, ...);

// print message and clear screen / update the screen.
void print_message_clear_display(const char *message, ...);

// print message and lock until user presses B.
void print_message_loop_lock(const char* message, ...);


/*
*   DATA READ / DATA WRITE.
*/

// read from file.
// out: data to be written to.
// size: size of data tp be read.
// offset: starting point of the data.
// f: the file to read the data from.
void read_file(void *out, size_t size, uint64_t offset, FILE *f);

// read data from the selected protocal.
// out: data to be written to.
// size: size of the data to read.
// offset: starting point of the data.
// f: optional file, only used if protocal is sd install.
void read_data_from_protocal(void *out, size_t size, uint64_t offset, install_protocal_t *protocal);


/*
* DEBUG functions.
*/

// dump data to a file.
// buf: data to dump.
// buf_size: size of data to dump.
// path: file to dump data to.
// mode: file mode. supported modes are w, wb, w+, wb+
size_t debug_dump_info(const void *buf, size_t buf_size, const char *path, const char *mode);


/*
*   MISC functions.
*/

//
void *mem_alloc(size_t size);

// switch util stuff.
bool magic_check(uint32_t magic1, uint32_t magic2);

// switch util stuff.
uint64_t media_to_offset(uint32_t offset);

#endif