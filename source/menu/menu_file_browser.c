#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <switch.h>

#include "nsp.h"
#include "xci.h"
#include "nca.h"
#include "fs.h"

#include "menu/menu_file_browser.h"
#include "menu/menu_install_location.h"
#include "util.h"


#define MAX_FILES   10000   //10,000


// GLOBALS
file_info_t *file_info[MAX_FILES];
folder_info_t *folder_info = NULL;

static char pwd[BUFFER_MAX];
static uint16_t cursor = 0;
static uint8_t list_move = 0;


void free_nodes()
{
    for (unsigned int i = 0; i < folder_info->total; i++)
    {
        free(file_info[i]);
        file_info[i] = NULL;
    }

    if (folder_info != NULL)
    {
        free(folder_info);
        folder_info = NULL;
    }
}

void reset_values()
{
    folder_info->total = 0;
    folder_info->total_files = 0;
    folder_info->total_folders = 0;
    folder_info->total_selected = 0;

    folder_info->total_nca = 0;
    folder_info->total_nsp = 0;
    folder_info->total_nsz = 0;
    folder_info->total_xci = 0;
    folder_info->total_xcz = 0;
}

void set_file_ext_colour(size_t file_in_array)
{
    if (!strcasecmp(file_info[file_in_array]->ext, NCA_FILE))       folder_info->total_nca++;
    else if (!strcasecmp(file_info[file_in_array]->ext, NSP_FILE))  folder_info->total_nsp++;
    else if (!strcasecmp(file_info[file_in_array]->ext, NSZ_FILE))  folder_info->total_nsz++;
    else if (!strcasecmp(file_info[file_in_array]->ext, XCI_FILE))  folder_info->total_xci++;
    else if (!strcasecmp(file_info[file_in_array]->ext, XCZ_FILE))  folder_info->total_xcz++;
}

void print_dir()
{
    consoleClear();
    printf("Welcome to total installer! 0.0.1 %s\n\n\n", pwd);

    for (size_t i = 0, j = list_move; i < folder_info->total && i < LIST_MAX; i++, j++)
    {
        if (file_info[j]->selected) printf("(*) ");
        if (j == cursor)    printf("[X] %s\n\n", file_info[j]->file_name);
        else                printf("[ ] %s\n\n", file_info[j]->file_name);
    }

    consoleUpdate(NULL);
}

void swap(int i, int j)
{
    // create a temporary file_info_t.
    file_info_t *temp;

    //swap...
    temp = file_info[i];
    file_info[i] = file_info[j];
    file_info[j] = temp;
}

void alphabetical_sort(int left, int right)
{
    if (left >= right) return;

    swap(left, (left + right)/2);
    int last = left;

    for (int i = left + 1; i <= right; i++)
    {
        if (file_info[left]->dir == NO && file_info[i]->dir == YES) swap(++last, i);
        else if (strcasecmp(file_info[i]->file_name, file_info[left]->file_name) < 0) swap(++last, i);
    }

    swap(left, last);
    alphabetical_sort(left, last -1);
    alphabetical_sort(last +1, right);
}

bool check_valid_ext(const char *ext)
{
    if (!strcasecmp(NCA_FILE, ext)) return true;
    if (!strcasecmp(NSP_FILE, ext)) return true;
    if (!strcasecmp(NSZ_FILE, ext)) return true;
    if (!strcasecmp(XCI_FILE, ext)) return true;
    if (!strcasecmp(XCZ_FILE, ext)) return true;
    return false;
}

void create_node(const char *folder_location)
{
    DIR *dir = opendir(folder_location);
    struct dirent *de;
    
    if (dir)
    {
        folder_info = malloc(sizeof(folder_info_t));
        reset_values();

        size_t n = 0;

        if (strcmp(pwd, ROOT))
        {
            // malloc the size of the node.
            file_info[n] = malloc(sizeof(file_info_t));

            // used to go back to the previous dir.
            strcpy(file_info[n]->file_name, "..");
            file_info[n]->dir = YES;
            file_info[n]->selected = false;
            n++;
        }

        // store all the information of each file in the directory.
        while ((de = readdir(dir)))
        {
            if (!is_dir(de->d_name) && !check_valid_ext(get_filename_ext(de->d_name))) continue;

            file_info[n] = malloc(sizeof(file_info_t));
            file_info[n]->selected = false;

            if (is_dir(de->d_name))
            {
                sprintf(file_info[n]->file_name, "/%s", de->d_name);
                folder_info->total_folders++;
                file_info[n]->dir = true;
                file_info[n]->file_size = 0;
            }
            else
            {
                strcpy(file_info[n]->file_name, de->d_name);
                folder_info->total_files++;
                file_info[n]->dir = false;
                strcpy(file_info[n]->ext, get_filename_ext(de->d_name));
                set_file_ext_colour(n);
                file_info[n]->file_size = get_filesize(de->d_name);
            }
            n++;
        }
        closedir(dir);

        // save the total number of files.
        folder_info->total = n;
        cursor = 0;
        list_move = 0;

        // sort if more than 1 file exists.
        if (folder_info->total > 1) alphabetical_sort(0, folder_info->total - 1);
    }
}

int enter_directory()
{
    // concatenate 2 strings, with a '/' in the middle.
    char *full_path;
    if (!asiprintf(&full_path, "%s/%s", pwd, file_info[cursor]->file_name))
        return 1;

    // change directory to the new path.
    chdir(full_path);

    // set new pwd.
    memset(pwd, 0, sizeof(pwd));
    getcwd(pwd, sizeof(pwd));

    // freeeeeeeee...
    free_nodes();

    // create new file_info_t with the size of number_of_files.
    create_node(full_path);
    free(full_path);

    return 0;
}

void move_back_dir()
{
    if (!strcmp(file_info[0]->file_name, ".."))
    {
        cursor = 0;
        enter_directory();
    }
}

bool file_select()
{
    // if selected file is a dir, enter it.
    if (file_info[cursor]->dir == YES) return enter_directory();

    int location = select_install_location();
    if (location == -1)         return 0; // break;
    else if (location == -2)    return 1; // exit file browser.

    // TODO: add multi install.
    // can just add code from my old install ui...
    /*if (!file_info[cursor]->selected)
    {
        file_info[cursor]->selected = true;
        folder_info->total_selected++;
    }*/

    //while (folder_info->total_selected)
    {
        if (!strcasecmp(file_info[cursor]->ext, NCA_FILE))      nca_prepare_single_install(file_info[cursor]->file_name, location);
        else if (!strcasecmp(file_info[cursor]->ext, NSP_FILE)) nsp_setup_install(file_info[cursor]->file_name, location, SD_CARD_INSTALL);
        else if (!strcasecmp(file_info[cursor]->ext, NSZ_FILE)) nsp_setup_install(file_info[cursor]->file_name, location, SD_CARD_INSTALL);
        else if (!strcasecmp(file_info[cursor]->ext, XCI_FILE)) xci_setup_install(file_info[cursor]->file_name, location, SD_CARD_INSTALL);
        else if (!strcasecmp(file_info[cursor]->ext, XCZ_FILE)) xci_setup_install(file_info[cursor]->file_name, location, SD_CARD_INSTALL);
    }
    return 0;
}

void clear_multi_select()
{
    for (size_t i = 0; folder_info->total_selected; i++)
    {
        if (!file_info[i]->selected)
            continue;
        file_info[i]->selected = false;
        folder_info->total_selected--;
    }
}

void select_all()
{
    for (size_t i = 0; folder_info->total_selected != (folder_info->total_files); i++)
    {
        if (file_info[i]->dir || file_info[i]->selected)
            continue;
        file_info[i]->selected = true;
        folder_info->total_selected++;
    }
}

int directory_menu()
{
    getcwd(pwd, sizeof(pwd));
    create_node(pwd);
    print_dir();

    while (appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // scroll up...
        if (kDown & KEY_UP)
        {
            cursor = move_cursor_up(cursor, folder_info->total);
            list_move = list_move_up(list_move, cursor, folder_info->total, LIST_MAX);
            print_dir();
        }

        // scroll down...
        if (kDown & KEY_DOWN)
        {
            cursor = move_cursor_down(cursor, folder_info->total);
            list_move = list_move_down(list_move, cursor, LIST_MAX);
            print_dir();
        }

        if (kDown & KEY_A)
        {
            if (file_select() == 1) break;
            print_dir();
        }

        if (kDown & KEY_B)
        {
            if (folder_info->total_selected > 0) clear_multi_select();
            else move_back_dir();
            print_dir();
        }

        // multi-select.
        if (kDown & KEY_X && !file_info[cursor]->dir)
        {
            if (file_info[cursor]->selected)
            {
                folder_info->total_selected--;
                file_info[cursor]->selected = false;
            }
            else
            {
                folder_info->total_selected++;
                file_info[cursor]->selected = true;
            }
            print_dir();
        }

        // select / de-select all.
        if (kDown & KEY_Y)
        {
            if (folder_info->total_selected == folder_info->total_files) clear_multi_select();
            else select_all();
            print_dir();
        }

        // exit app.
        if (kDown & KEY_PLUS) break;
    }

    free_nodes();
    return 0;
}