#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <dirent.h>
#include <unistd.h>
#include <switch.h>

#include "util.h"
#include "fs.h"
#include "usb.h"
#include "network.h"


size_t debug_dump_info(const void *buf, size_t buf_size, const char *path, const char *mode)
{
    size_t size_written = 0;
    FILE *f = fopen(path, mode);
    if (!f)
        return size_written;
    size_written = fwrite(buf, buf_size, 1, f);
    fclose(f);
    return size_written;
}

const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot + 1;
}

unsigned int move_cursor_up(unsigned int cursor, unsigned int cursor_max)
{
    if (cursor == 0)
        cursor = cursor_max - 1;
    else
        cursor--;
    return cursor;
}

unsigned int move_cursor_down(unsigned int cursor, unsigned int cursor_max)
{
    if (cursor == cursor_max - 1)
        cursor = 0;
    else
        cursor++;
    return cursor;
}

unsigned int list_move_up(unsigned int list_move, unsigned int cursor, unsigned int number_of_files, unsigned int list_max)
{
    if (cursor == list_move - 1)
        list_move--;
    else if
        (cursor == number_of_files - 1 && list_max <= number_of_files) list_move = cursor - (list_max - 1);
    return list_move;
}

unsigned int list_move_down(unsigned int list_move, unsigned int cursor, unsigned int list_max)
{
    if (cursor == (list_max - 1) + (list_move + 1))
        list_move++;
    else if (cursor == 0)
        list_move = 0;
    return list_move;
}

size_t get_filesize(const char *file)
{
    size_t size = 0;
    FILE *f = fopen(file, "r");
    if (!f)
    {
        printf("failed to open file!\n");
        return size;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fclose(f);
    return size;
}

FILE *open_file(const char *file, const char *mode)
{
    FILE *f = fopen(file, mode);
    if (f)
        return f;
    print_message_loop_lock("failed to open file %s\npress b to exit\n", file);
    return NULL;
}

DIR *open_dir(const char *directory)
{
    DIR *dir = opendir(directory);
    if (dir)
        return dir;
    print_message_loop_lock("failed to open directory %s\npress b to exit\n", directory);
    return NULL;
}

void read_file(void *out, size_t size, u_int64_t offset, FILE *f)
{
    fseek(f, offset, SEEK_SET);
    fread(out, size, 1, f);
}

bool is_dir(const char *folder_to_check)
{
    DIR *dir = opendir(folder_to_check);
    if (!dir)
        return false;
    closedir(dir);
    return true;
}

bool check_if_file_exists(const char *file)
{
    FILE *f = fopen(file, "r");
    if (!f)
        return false;
    fclose(f);
    return true;
}

bool check_if_dir_exists(const char *directory)
{
    DIR *dir = opendir(directory);
    if (!dir)
        return false;
    closedir(dir);
    return true;
}

bool change_dir(const char *path)
{
    if (!check_if_dir_exists(path))
        create_dir(path);
    int res = chdir(path);
    if (res == 0)
        return true;
    return false;
}

size_t scan_dir(const char *directory)
{
    size_t number_of_files = 0;
    struct dirent *de;
    DIR *dir = open_dir(directory);
    if (!dir)
        return number_of_files;

    while ((de = readdir(dir)))
    {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;
        number_of_files++;
    }

    closedir(dir);
    return number_of_files;
}

size_t scan_dir_recursive(const char *directory)
{
    size_t num = 0;
    struct dirent *de;
    DIR *dir = open_dir(directory);
    if (!dir)
        return num;
        
    while ((de = readdir(dir)))
    {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;

        char *full_path;
        if (!asiprintf(&full_path, "%s/%s", directory, de->d_name))
            return num;

        if (is_dir(de->d_name))
            num += get_foldersize(de->d_name);

        num++;
        free(full_path);
    }
    closedir(dir);
    return num;
}

size_t get_foldersize(const char *directory)
{
    size_t size = 0;
    DIR *dir = opendir(directory);
    struct dirent *de;
    if (!dir) return size;

    while ((de = readdir(dir)))
    {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;

        char *full_path;
        if (!asiprintf(&full_path, "%s/%s", directory, de->d_name))
            return size;

        if (is_dir(full_path))
            size += get_foldersize(full_path);
        else
            size += get_filesize(full_path);

        free(full_path);
    }
    return size;
}

bool create_file(const char *file)
{
    if (check_if_file_exists(file))
        return true;
    FILE *f = fopen(file, "wb");
    if (!f)
        return false;
    fclose (f);
    return true;
}

bool create_dir(const char *dir)
{
    if (check_if_dir_exists(dir))
        return true;
    int res = mkdir(dir, 0777);
    if (res == 0)
        return true;
    return false;
}

bool delete_file(const char *file)
{
    if (!check_if_file_exists(file))
        return true;
    int res = remove(file);
    if (res == 0)
        return true;
    return false;
}

void delete_dir(const char *directory)
{
    struct dirent *de;
    DIR *dir = open_dir(directory);
    if (!dir)
        return;

    while ((de = readdir(dir)))
    {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;

        char *full_path;
        if (!asiprintf(&full_path, "%s/%s", directory, de->d_name))
            return;

        if (is_dir(full_path))
            delete_dir(full_path);
        else
            delete_file(full_path);

        free(full_path);
    }
    closedir(dir);
    rmdir(directory);
}

void copy_file(const char *src, char *dest)
{
    FILE *srcfile = fopen(src, "rb");
    FILE *newfile = fopen(dest, "wb");

    if (srcfile && newfile)
    {
        void *buf = malloc(0x800000);
        size_t bytes;           // size of the file to write (8MiB or filesize max)

        while (0 < (bytes = fread(buf, 1, 0x800000, srcfile)))
            fwrite(buf, bytes, 1, newfile);
        free(buf);
    }
    fclose(srcfile);
    fclose(newfile);
}

void copy_folder(const char *src, char *dest)
{
    if (!create_dir(dest))
        return;
    struct dirent *de;
    DIR *dir = open_dir(src);
    if (!dir)
        return;

    while ((de = readdir(dir)))
    {
        char buffer[512];
        snprintf(buffer, sizeof(buffer), "%s/%s", dest, de->d_name);

        // check if the file is a directory.
        if (is_dir(de->d_name))
            create_dir(buffer);
        else
            copy_file(de->d_name, buffer);
    }
    closedir(dir);
}

void move_file(const char *src, char *dest)
{
    rename(src, dest);
}

void move_folder(const char *src, char *dest)
{
    DIR *dir = open_dir(src);
    struct dirent *de;
    create_dir(dest);

    while ((de = readdir(dir)))
    {
        char *full_path;
        if (!asiprintf(&full_path, "%s/%s", src, de->d_name))
            return;

        // check if the file is a directory.
        if (is_dir(de->d_name))
            create_dir(full_path);
        else
            move_file(de->d_name, full_path);

        free(full_path);
    }
    closedir(dir);
}

bool parse_search_from_file(const char *file, const char *filter, char *out_string)
{    
    FILE *fp = open_file(file, "r");
    if (!fp)
        return false;

    char c;
    while ((c = fgetc(fp)) != EOF)
    {
        if (c == *filter)
        {
            for (int i = 0, len = strlen(filter) - 1; c == filter[i]; i++)
            {
                c = fgetc(fp);
                if (i == len)
                {
                    for (int j = 0; c != '\"'; j++)
                    {
                        out_string[j] = c;
                        out_string[j + 1] = '\0';
                        c = fgetc(fp);
                    }
                    fclose(fp);
                    return true;
                }
            }
        }
    }
    print_message_loop_lock("unable to parse search using filter %s\npress b to exit\n", filter);
    fclose(fp);
    return false;
}

void keyboard(char *buffer, const char *clipboard, ...)
{
    char new_message[FS_MAX_PATH];
    va_list arg;
    va_start(arg, clipboard);
    vsprintf(new_message, clipboard, arg);
    va_end(arg);

    SwkbdConfig config;
    swkbdConfigMakePresetDefault(&config);
    swkbdConfigSetInitialText(&config, new_message);
    swkbdCreate(&config, 0);
    swkbdShow(&config, buffer, 256);
    swkbdClose(&config);
}

void print_message_display(const char* message, ...)
{
    char new_message[FS_MAX_PATH];
    va_list arg;
    va_start(arg, message);
    vsprintf(new_message, message, arg);
    va_end(arg);

    printf("%s", new_message);
    consoleUpdate(NULL);
}

void print_message_clear_display(const char *message, ...)
{
    char new_message[FS_MAX_PATH];
    va_list arg;
    va_start(arg, message);
    vsprintf(new_message, message, arg);
    va_end(arg);

    consoleClear();
    printf("%s", new_message);
    consoleUpdate(NULL);
}

void print_message_loop_lock(const char* message, ...)
{
    char new_message[FS_MAX_PATH];
    va_list arg;
    va_start(arg, message);
    vsprintf(new_message, message, arg);
    va_end(arg);

    printf("%s", new_message);
    consoleUpdate(NULL);
    while (appletMainLoop()) //1:55
    {
        hidScanInput();
        if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B) break;
    }
}

void read_data_from_protocal(InstallProtocal mode, void *out, size_t size, u_int64_t offset, FILE *f, FsFile *f2)
{
    u_int8_t *data_temp = memalign(0x1000, size);

    switch (mode)
    {
        case SD_CARD_INSTALL:
        {
            read_file(data_temp, size, offset, f);
            break;
        }
        case USB_INSTALL:
        {
            usb_read(data_temp, size, offset);
            break;
        }
        case NTWRK_INSTALL:
        {
            ntwrk_setup_download(data_temp, size, offset);
            break;
        }
        case GC_INSTALL:
        {
            fs_read_file(data_temp, size, offset, FsReadOption_None, f2);
            break;
        }
    }

    memcpy(out, data_temp, size);
    free(data_temp);
}

void *mem_alloc(size_t size)
{
    void *mem = malloc(size);
    if (mem == NULL)
        print_message_loop_lock("failed to alloc mem with size %lu\n", size);
    memset(mem, 0, size);
    return mem;
}


/*
*   NX functions.
*/