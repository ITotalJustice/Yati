#ifndef _DIR_H_
#define _DIR_H_

#include <stdbool.h>

#define NCA_FILE    "nca"
#define NSP_FILE    "nsp"
#define NSZ_FILE    "nsz"
#define XCI_FILE    "xci"
#define XCZ_FILE    "xcz"

#define YES         1
#define NO          0

#define ROOT        "sdmc:/"

#define BUFFER_MAX          512


typedef struct file_info
{
    char file_name[BUFFER_MAX];
    char ext[5];
    bool dir;
    bool selected;
    double file_size;
    u8 ext_colour;
} file_info_t;

typedef struct folder_info
{
    size_t total;
    size_t total_folders;
    size_t total_files;
    size_t total_selected;

    size_t total_nca;
    size_t total_nsp;
    size_t total_nsz;
    size_t total_xci;
    size_t total_xcz;
} folder_info_t;


// print all files in a dir.
void print_dir(void);

// create a node sizeof (node * number_of_files).
void create_node(const char *folder_location);

//
int enter_directory(void);

//
void move_back_dir(void);

//
bool file_select(void);

//
void free_nodes(void);

//
int directory_menu(void);

#endif