#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <switch.h>

#include "network.h"
#include "unzip.h"
#include "util.h"


#define APP_VERSION             "0.1.3++"
#define CURSOR_LIST_MAX         2

#define GBATEMP_FILTER_STRING   "attachments/"
#define GITHUB_FILTER_STRING    "browser_download_url\":\""

#define GBATEMP_URL             "https://gbatemp.net/attachments/"
#define SIGS_URL                "https://gbatemp.net/threads/i-heard-that-you-guys-need-some-sweet-patches-for-atmosphere.521164/"
#define JOON_SIGS_URL           "https://api.github.com/repos/Joonie86/hekate/releases/latest"


enum
{
    UP_SIGS,
    UP_JOONIE
} update;


void sigpatch_menu_refresh(int cursor)
{
    consoleClear();

    char *option_list[] =
    {
        "= Update Sigpatches (For Atmosphere Users)",
        "= Update Sigpatches (For Hekate / Kosmos Users)"
    };

    printf("\x1B[36mSigpatch-Updater: v%s.\x1B[37m\n\n\n", APP_VERSION);
    printf("Press (A) to select option\n\n");
    printf("Press (+) to exit\n\n\n");

    for (uint8_t i = 0; i < CURSOR_LIST_MAX; i++)
    {
        if (cursor != i)
            printf("[ ] %s\n\n", option_list[i]);
        else
            printf("[X] %s\n\n", option_list[i]);
    }
    consoleUpdate(NULL);
}

int sigpatch_menu()
{
    // set the cursor position to 0
    short cursor = 0;

    // main menu
    sigpatch_menu_refresh(cursor);

    // muh loooooop
    while (appletMainLoop())
    {
        hidScanInput();
        uint64_t kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // move cursor up...
        if (kDown & KEY_UP)
        {
            cursor = move_cursor_up(cursor, CURSOR_LIST_MAX);
            sigpatch_menu_refresh(cursor);
        }

        // move cursor down...
        if (kDown & KEY_DOWN)
        {
            cursor = move_cursor_down(cursor, CURSOR_LIST_MAX);
            sigpatch_menu_refresh(cursor);
        }

        if (kDown & KEY_A)
        {
            switch (cursor)
            {
                case UP_SIGS:
                {
                    if (ntwrk_quick_download(SIGS_URL, DataSize_1MiB, TEMP_FILE))
                    {
                        char file_name[FILENAME_MAX];
                        if (parse_search_from_file(TEMP_FILE, GBATEMP_FILTER_STRING, file_name))
                        {
                            char new_url[FILENAME_MAX];
                            if (snprintf(new_url, FILENAME_MAX, "%s%s", GBATEMP_URL, file_name))
                            {
                                if (ntwrk_quick_download(new_url, DataSize_1MiB, TEMP_FILE))
                                {
                                    unzip(TEMP_FILE);
                                }
                            }
                        }
                    }
                    break;
                }

                case UP_JOONIE:
                {
                    if (ntwrk_quick_download(JOON_SIGS_URL, DataSize_1MiB, TEMP_FILE))
                    {
                        char new_url[FILENAME_MAX];
                        if (parse_search_from_file(TEMP_FILE, GITHUB_FILTER_STRING, new_url))
                        {
                            if (ntwrk_quick_download(new_url, DataSize_1MiB, TEMP_FILE))
                            {
                                unzip(TEMP_FILE);
                            }
                        }
                    }
                    break;
                }
            }
            remove(TEMP_FILE);
        }
        
        // exit...
        if (kDown & KEY_PLUS || kDown & KEY_B)
            break;
    }
    return 0;
}