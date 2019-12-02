#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "es.h"
#include "ns.h"
#include "nacp.h"
#include "ncm.h"
#include "util.h"
#include "ticket.h"


void ticket_menu_refresh_screen(tik_struct_t *tik, u32 cursor, u32 list_move)
{
    consoleClear();

    printf("Ticket Menu:\n\n");
    printf("Press X to delete a ticket.\tPress B / + to quit\n");

    for (u32 i = 0, j = list_move; i < tik->total && i < LIST_MAX; i++, j++)
    {
        if (cursor == j)
            printf("\n\n[X] %s", tik->info[j].name);
        else
            printf("\n\n[ ] %s", tik->info[j].name);
    }

    consoleUpdate(NULL);
}


int ticket_menu()
{
    tik_struct_t tik;
    if (!ticket_get_tik_total(&tik))
        return 1;

    tik.info = malloc(tik.total * sizeof(tik_info_struct_t));
    memset(tik.info, 0, tik.total);

    if (!ticket_setup_tik_info(&tik))
    {
        free(tik.info);
        return 1;
    }
    
    u32 cursor = 0;
    u32 list_move = 0;

    ticket_menu_refresh_screen(&tik, cursor, list_move);

    while (appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // scroll up...
        if (kDown & KEY_UP)
        {
            cursor = move_cursor_up(cursor, tik.total);
            list_move = list_move_up(list_move, cursor, tik.total, LIST_MAX);
            ticket_menu_refresh_screen(&tik, cursor, list_move);
        }

        // scroll down...
        if (kDown & KEY_DOWN)
        {
            cursor = move_cursor_down(cursor, tik.total);
            list_move = list_move_down(list_move, cursor, LIST_MAX);
            ticket_menu_refresh_screen(&tik, cursor, list_move);
        }

        if (kDown & KEY_X)
        {
            ticket_delete(&tik.info[cursor].rights_id, tik.info[cursor].type);
        }

        if (kDown &KEY_B || kDown &KEY_PLUS)
            break;
    }


    // clean the exit.
    free(tik.info);
    return 0;
}