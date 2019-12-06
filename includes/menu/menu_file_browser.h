#ifndef _DIR_H_
#define _DIR_H_

#include <stdint.h>
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


typedef enum
{
    FileType_Dir = 0x0,
    FileType_File = 0x1
} FileType;

typedef enum
{
    FileType_None = 0x0,
    FileType_Ignore = 0x1,

    FileType_Txt = 0x10,
    FileType_Ini = 0x11,
    FileType_Html = 0x12,

    FileType_Zip = 0x20,
    FileType_7zip = 0x21,
    FileType_Rar = 0x22,

    FileType_Mp3 = 0x30,
    FileType_Mp4 = 0x31,
    FileType_Mkv = 0x32,

    FileType_Nro = 0x40,
    FileType_Nso = 0x41,
    
    FileType_Nca = 0x50,
    FileType_Nsp = 0x51,
    FileType_Xci = 0x52,
    FileType_Ncz = 0x53,
    FileType_Nsz = 0x54,
    FileType_Xcz = 0x55,
} FileExtentionType;

typedef enum
{
    FileType_Dir,
    FileType_Compressed,
    FileType_Music,
    FileType_Movie,
    FileType_Homebrew,
    FileType_Game,
    FileType_Other
} FileCatagory;

typedef enum
{
    FileSelected_No,
    FileSelected_Yes
} FileSelected;

typedef enum
{
    FileSizeType_Small,
    FileSizeType_Large
} FileSizeType;

typedef struct file_info
{
    char file_name[BUFFER_MAX];
    char ext[5];
    bool dir;
    bool selected;
    double file_size;
    uint8_t ext_colour;
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