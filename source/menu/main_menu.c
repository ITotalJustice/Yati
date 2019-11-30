#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "util.h"
#include "menu/menu_file_browser.h"
#include "menu/menu_install_location.h"
#include "menu/menu_sigpatch.h"
#include "menu/menu_remote_install.h"
#include "menu/menu_gc.h"
#include "menu/menu_tickets.h"


enum
{
    SD      = 0,
    GC      = 1,
    USB     = 2,
    HTTP    = 3,
    TICKET  = 4,
    SIGS    = 5,
} MainMenuSelected;


void print_main_menu(const char **location_options, unsigned int location_cursor, unsigned int list_max)
{
    consoleClear();
    printf("YATI (yet another title installer). Select an option:\n\n\n");

    for (u8 i = 0; i < list_max; i++)
    {
        if (location_cursor == i)
            printf("[X] %s\n\n", location_options[i]);
        else
            printf("[ ] %s\n\n", location_options[i]);
    }
    consoleUpdate(NULL);
}

void menu_main()
{
    u8 main_menu_cursor = 0;
    u8 main_menu_cursor_max = 6;
    const char *main_menu_option[] =
    {
        "SdCard install",
        "GameCard install",
        "USB install",
        "Network install",
        "Ticket browser",
        "Sigpatch update"
    };

    print_main_menu(main_menu_option, main_menu_cursor, main_menu_cursor_max);

    while (appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_UP)
        {
            main_menu_cursor = move_cursor_up(main_menu_cursor, main_menu_cursor_max);
            print_main_menu(main_menu_option, main_menu_cursor, main_menu_cursor_max);
        }

        if (kDown & KEY_DOWN)
        {
            main_menu_cursor = move_cursor_down(main_menu_cursor, main_menu_cursor_max);
            print_main_menu(main_menu_option, main_menu_cursor, main_menu_cursor_max);
        }

        if (kDown & KEY_A)
        {
            int location_res;
            switch (main_menu_cursor)
            {
                case SD:
                {
                    if (directory_menu())
                        return;
                    break;
                }
                case GC:
                {
                    switch (location_res = select_install_location())
                    {
                        case -1: break;
                        case -2: return;
                        default: gc_menu_start(location_res);
                    }
                    break;
                }
                case USB:
                {
                    switch (location_res = select_install_location())
                    {
                        case -1: break;
                        case -2: return;
                        default: remote_install_start(location_res, USB_INSTALL);
                    }
                    break;
                }
                case HTTP:
                {
                    switch (location_res = select_install_location())
                    {
                        case -1: break;
                        case -2: return;
                        default: remote_install_start(location_res, NTWRK_INSTALL);
                    }
                    break;
                }
                case TICKET:
                {
                    ticket_menu();
                }
                case SIGS:
                {
                    sigpatch_menu();
                }
            }
            print_main_menu(main_menu_option, main_menu_cursor, main_menu_cursor_max);
        }

        // exit
        if (kDown & KEY_PLUS || kDown & KEY_B)
            break;
    }
}