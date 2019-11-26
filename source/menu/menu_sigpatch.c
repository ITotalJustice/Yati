#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <switch.h>

#include "network.h"
#include "unzip.h"
#include "util.h"


#define APP_VERSION             "0.1.2"
#define CURSOR_LIST_MAX         2

#define GBATEMP_FILTER_STRING   "attachments/"
#define GITHUB_FILTER_STRING    "browser_download_url\":\""

#define GBATEMP_URL             "https://gbatemp.net/attachments/"
#define SIGS_URL                "https://gbatemp.net/threads/i-heard-that-you-guys-need-some-sweet-patches-for-atmosphere.521164/"
#define JOON_SIGS_URL           "https://api.github.com/repos/Joonie86/hekate/releases/latest"


bool parse_search(const char *parse_string, const char *filter, char* new_string)
{    
    FILE *fp = fopen(parse_string, "r");
    if (!fp) return false;

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
                        new_string[j] = c;
                        new_string[j + 1] = '\0';
                        c = fgetc(fp);
                    }
                    fclose(fp);
                    remove(parse_string);
                    return true;
                }
            }
        }
    }
    print_message_loop_lock("unable to parse search using filter %s\npress b to exit\n", filter);
    return false;
}

void refreshScreen(int cursor)
{
    consoleClear();

    char *option_list[] = { "= Update Sigpatches (For Atmosphere Users)", \
                            "= Update Sigpatches (For Hekate / Kosmos Users)" };

    printf("\x1B[36mSigpatch-Updater: v%s.\x1B[37m\n\n\n", APP_VERSION);
    printf("Press (A) to select option\n\n");
    printf("Press (+) to exit\n\n\n");

    for (int i = 0; i < (CURSOR_LIST_MAX); i++)
    {
        if (cursor != i)    printf("[ ] %s\n\n", option_list[i]);
        else                printf("[X] %s\n\n", option_list[i]);
    }
    consoleUpdate(NULL);
}

int sigpatch_menu()
{
    // set the cursor position to 0
    short cursor = 0;

    // main menu
    refreshScreen(cursor);

    // muh loooooop
    while (appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // move cursor up...
        if (kDown & KEY_UP)
        {
            cursor = move_cursor_up(cursor, CURSOR_LIST_MAX);
            refreshScreen(cursor);
        }

        // move cursor down...
        if (kDown & KEY_DOWN)
        {
            cursor = move_cursor_down(cursor, CURSOR_LIST_MAX);
            refreshScreen(cursor);
        }

        if (kDown & KEY_A)
        {
            switch (cursor)
            {
            case UP_SIGS:
                if (ntwrk_quick_download(SIGS_URL, 0x100000, TEMP_FILE))
                {
                    char file_name[FILENAME_MAX];
                    if (parse_search(TEMP_FILE, GBATEMP_FILTER_STRING, file_name))
                    {
                        char new_url[FILENAME_MAX];
                        if (snprintf(new_url, FILENAME_MAX, "%s%s", GBATEMP_URL, file_name))
                            if (ntwrk_quick_download(new_url, 0x100000, TEMP_FILE))
                                unzip(TEMP_FILE);
                    }
                }
                break;

            case UP_JOONIE:
                if (ntwrk_quick_download(JOON_SIGS_URL, 0x100000, TEMP_FILE))
                {
                    char new_url[FILENAME_MAX];
                    if (parse_search(TEMP_FILE, GITHUB_FILTER_STRING, new_url))
                        if (ntwrk_quick_download(new_url, 0x100000, TEMP_FILE))
                            unzip(TEMP_FILE);
                }
                break;
            }
            remove(TEMP_FILE);
        }
        
        // exit...
        if (kDown & KEY_PLUS || kDown & KEY_B) break;
    }
    return 0;
}