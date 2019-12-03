// TODO:
/*
    standard crypto (ticketless).
    easy access functions for
        ticket dumping.
        ticket listing.
        ticket deleting.
    have a struct which will contain name belonging to each ticket and rights_id (for deleting / dumping).
    have menu_tickets.c call all functions from here.
        menu should only be a browser for tickets, functions should be inside this file.
    probably some other stuff which ive forgotten...
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "ticket.h"
#include "es.h"
#include "nacp.h"
#include "crypto.h"
#include "util.h"


void *ticket_decrypt_title_key(const void *title_key, void *out, size_t size)
{
    return crypto_encrypt_decrypt_aes_cbc(title_key, out, NULL, size, NULL, EncryptMode_Decrypt);
}

void *ticket_encrypt_title_key(const void *title_key, void *out, size_t size)
{
    return crypto_encrypt_decrypt_aes_cbc(title_key, out, NULL, size, NULL, EncryptMode_Encrypt);
}

bool ticket_get_tik_total(ticket_struct_t *tik)
{
    if (!(tik->common_total = es_count_common_tik()) && !(tik->personalised_total = es_count_personailsed_tik()))
    {
        print_message_loop_lock("no tickets installed\n");
        return false;
    }
    tik->total = tik->common_total + tik->personalised_total;
    return true;
}

bool ticket_read_common(ticket_struct_t *tik)
{
    FsRightsId *rights_id = malloc(tik->common_total * sizeof(FsRightsId));
    memset(rights_id, 0, tik->common_total * sizeof(FsRightsId));

    if (R_FAILED(es_list_common_tik(rights_id, tik->common_total)))
    {
        free(rights_id);
        return false;
    }
    
    // fill in tik_info.
    for (u32 i = 0; i < tik->common_total; i++)
    {
        NacpLanguageEntry *lang_entry = NULL;
        nacp_get_lang_entry_from_rights_id(rights_id[i], &lang_entry);

        if (lang_entry == NULL)
            snprintf(tik->info[i].name, 0x200, "%016lu", (u64)rights_id[i].c);

        else
            snprintf(tik->info[i].name, 0x200, "%s", lang_entry->name);
        
        memcpy(&tik->info[i].rights_id, &rights_id[i], sizeof(FsRightsId));
        tik->info[i].type = TicketType_Common;
    }
    
    // we are done with this now...
    free(rights_id);
    return true;
}

bool ticket_read_personalised(ticket_struct_t *tik)
{
    FsRightsId *rights_id = malloc(tik->personalised_total * sizeof(FsRightsId));
    memset(rights_id, 0, tik->personalised_total * sizeof(FsRightsId));

    if (R_FAILED(es_list_common_tik(rights_id, tik->personalised_total)))
    {
        free(rights_id);
        return false;
    }
    
    // fill in tik_info.
    for (u32 i = tik->common_total; i < tik->total; i++)
    {
        NacpLanguageEntry *lang_entry = NULL;
        nacp_get_lang_entry_from_rights_id(rights_id[i], &lang_entry);

        if (lang_entry == NULL)
            snprintf(tik->info[i].name, 0x200, "%016lx", *(u64 *)rights_id[i].c);

        else
            snprintf(tik->info[i].name, 0x200, "%s", lang_entry->name);
        
        memcpy(&tik->info[i].rights_id, &rights_id[i], sizeof(FsRightsId));
        tik->info[i].type = TicketType_Personalised;
    }
    
    // we are done with this now...
    free(rights_id);
    return true;
}

bool ticket_setup_tik_info(ticket_struct_t *tik)
{
    if (tik->common_total)
        if (!ticket_read_common(tik))
            return false;

    if (tik->personalised_total)
        if (!ticket_read_personalised(tik))
            return false;

    return true;
}

Result ticket_delete(const FsRightsId *rights_id, TicketType type)
{
    switch (type)
    {
        case (TicketType_Common):
            return es_delete_common_tik(rights_id);

        case (TicketType_Personalised):
            return es_delete_personalised_tik(rights_id);

        default:
            print_message_loop_lock("unkown TicketType\n");
            return 1;
    }
}