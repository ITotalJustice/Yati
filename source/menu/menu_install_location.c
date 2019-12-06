#include <stdio.h>
#include <stdint.h>
#include <switch.h>

#include "util.h"
#include "menu/main_menu.h"


int select_install_location()
{
    uint8_t location_cursor = 0;
    uint8_t location_list_max = 2;
    uint8_t install_location[] = { NcmStorageId_BuiltInUser, NcmStorageId_SdCard };
    const char *location_options[] = { "NAND", "SD_CARD" };
    
    print_main_menu(location_options, location_cursor, location_list_max);

    while (1)
    {
        hidScanInput();
        uint64_t kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_UP)
        {
            location_cursor = move_cursor_up(location_cursor, location_list_max);
            print_main_menu(location_options, location_cursor, location_list_max);
        }

        if (kDown & KEY_DOWN)
        {
            location_cursor = move_cursor_down(location_cursor, location_list_max);
            print_main_menu(location_options, location_cursor, location_list_max);
        }                 

        if (kDown & KEY_A)
            return install_location[location_cursor];

        if (kDown & KEY_B)
            break;

        if (kDown & KEY_PLUS)
            return -2;
    }
    return -1;
}