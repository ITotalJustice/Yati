#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <minizip/unzip.h>
#include <string.h>

#include "unzip.h"
#include "util.h"

#define WRITEBUFFERSIZE 0x400 // 1KB


bool unzip_is_folder(const char *file)
{
    // check if the string ends with a /, if so, then its a directory.
    if ((file[strlen(file) - 1]) == '/') return true;
    return false;
}

bool unzip_get_global_info()
{
    return false;
}

bool unzip_open_file(unzFile zfile)
{
    if (UNZ_OK == unzOpenCurrentFile(zfile))
        return true;
    print_message_loop_lock("failed to open current zip file\npressb to exit\n");
    return false;
}

bool unzip_close_file(unzFile zfile)
{
    if (UNZ_OK == unzCloseCurrentFile(zfile))
        return true;
    print_message_loop_lock("failed to close current file in unzip\npress b to exit\n");
    return false;
}

bool unzip_get_file_info(unzFile zfile, unz_file_info *file_info, char *name_out, size_t size)
{
    if (UNZ_OK == unzGetCurrentFileInfo(zfile, file_info, name_out, size, NULL, 0, NULL, 0))
        return true;
    print_message_loop_lock("failed to get current file info in zip\npress b to exit\n");
    return false;
}

bool unzip_next_file(unzFile zfile)
{
    if (UNZ_OK == unzGoToNextFile(zfile))
        return true;
    print_message_loop_lock("failed to go to next file in unzip\npress b to exit\n");
    return false;
}

bool unzip_contents(unzFile zfile)
{
    char filename_inzip[FILENAME_MAX];
    unz_file_info file_info;
    if (!unzip_get_file_info(zfile, &file_info, filename_inzip, FILENAME_MAX))
        return false;

    // check if the string ends with a /, if so, then its a directory.
    if (unzip_is_folder(filename_inzip)) { if (!check_if_dir_exists(filename_inzip)) if (!create_dir(filename_inzip)) return false; }

    else
    {
        FILE *outfile = fopen(filename_inzip, "wb");
        if (!outfile)
        {
            print_message_loop_lock("failed to create file %s\npress B to exit", filename_inzip);
            return false;
        }

        void *buf = malloc(WRITEBUFFERSIZE);

        print_message_display("writing file: %s\r", filename_inzip);

        for (size_t j = unzReadCurrentFile(zfile, buf, WRITEBUFFERSIZE); j > 0; j = unzReadCurrentFile(zfile, buf, WRITEBUFFERSIZE))
            fwrite(buf, 1, j, outfile);

        fclose(outfile);
        free(buf);
    }
    return true;
}

int unzip(const char *file)
{
    unzFile zfile = unzOpen(file);
    unz_global_info gi;
    if (unzGetGlobalInfo(zfile, &gi))
    {
        print_message_loop_lock("failed to get info of zip file\n");
        return false;
    }

    if (!change_dir("/"))
    {
        print_message_loop_lock("failed to change to root dir\n");
        return false;
    }

    for (uint32_t i = 0; i < gi.number_entry; i++)
    {
        if (!unzip_open_file(zfile))
            break;
        bool res = unzip_contents(zfile);
        
        if (!res)
            break;
        if (!unzip_next_file(zfile))
            break;
    }

    unzClose(zfile);
    print_message_display("\nfinished!\n\nRemember to reboot for the patches to be loaded!\n");

    change_dir("/switch/yati");
    return 0;
}