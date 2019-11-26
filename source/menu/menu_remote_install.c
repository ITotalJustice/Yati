#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <switch.h>

#include "network.h"
#include "usb.h"
#include "util.h"
#include "nsp.h"
#include "xci.h"
#include "pfs0.h"


#define HTTP_TEXT_FILE "http_game.txt"

// globals
static char url[FS_MAX_PATH];


bool http_check_if_file_exists()
{
    print_message_display("checking for http file...\n");
    FILE *f = open_file(HTTP_TEXT_FILE, "r");
    if (!f) return false;

    memset(url, 0, FS_MAX_PATH);
    fread(url, get_filesize(HTTP_TEXT_FILE), 1, f);
    fclose(f);
    print_message_display("url is %s\n", url);
    return true;
}

bool remote_install_start(NcmStorageId storage_id, InstallProtocal mode)
{
    u_int32_t magic = 0;

    switch (mode)
    {
        case SD_CARD_INSTALL:
            print_message_loop_lock("this shouldn't happen...\n");
            return false;
        case NTWRK_INSTALL:
            if (!http_check_if_file_exists()) return false;
            if (!ntwrk_start(url)) return false;
            break;
        case USB_INSTALL:
            // init usb here
            break;
    }

    print_message_display("getting file magic...\n");
    read_data_from_protocal(mode, &magic, sizeof(u_int32_t), 0, NULL);

    if (pfs0_check_valid_magic(magic))
    {
        print_message_display("selected nsp install\n");
        nsp_setup_install(NULL, storage_id, mode);
    }
    else
    {
        print_message_display("selected xci install\n");
        xci_setup_install(NULL, storage_id, mode);
    }

    //cleanup
    switch (mode)
    {
        case SD_CARD_INSTALL:
            print_message_loop_lock("this shouldn't happen...\n");
            return false;
        case NTWRK_INSTALL:
            ntwrk_exit();
            break;
        case USB_INSTALL:
            usb_exit();
            break;
    }

    return true;
}