#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdint.h>
#include <dirent.h>


#define TEMP_FILE "temp"


typedef enum
{
    SD_CARD_INSTALL = 0,
    USB_INSTALL     = 1,
    NTWRK_INSTALL   = 2,
} InstallProtocal;


/*
*   CURSOR FUNCTIONS.
*/

// move the cursor up / down.
unsigned int move_cursor_up(unsigned int cursor, unsigned int cursor_max);
unsigned int move_cursor_down(unsigned int cursor, unsigned int cursor_max);

// move list up / down.
unsigned int list_move_up(unsigned int list_move, unsigned int cursor, unsigned int number_of_files, unsigned int list_max);
unsigned int list_move_down(unsigned int list_move, unsigned int cursor, unsigned int list_max);


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
void read_file(void *out, size_t size, u_int64_t offset, FILE *f);

// read data from the selected protocal.
// out: data to be written to.
// size: size of the data to read.
// offset: starting point of the data.
// f: optional file, only used if protocal is sd install.
void read_data_from_protocal(InstallProtocal mode, void *out, size_t size, u_int64_t offset, FILE *f);


/*
* DEBUG functions.
*/

// dump data to a file.
// buf: data to dump.
// buf_size: size of data to dump.
// path: file to dump data to.
// mode: file mode. supported modes are w, wb, w+, wb+
size_t debug_dump_info(const void *buf, size_t buf_size, const char *path, const char *mode);


#endif