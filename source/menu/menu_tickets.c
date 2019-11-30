#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "es.h"
#include "ns.h"
#include "nacp.h"
#include "ncm.h"
#include "util.h"


// awful code here
// don't code at 4am.

/*
typedef struct
{
    NcmRightsId ncm_rights;
    u64 app_id;
} temp_t;

typedef struct
{
    u32 common_total;
    u32 person_total;

    temp_t *common_tiks;
    temp_t *person_tiks;
} ticket_struct_t;


void ticket_struct_free(ticket_struct_t *ticket_struct)
{
    if (ticket_struct->common_tiks != NULL)
        free(ticket_struct->common_tiks);
    if (ticket_struct->person_tiks != NULL)
        free(ticket_struct->person_tiks);
}


bool ticket_setup(ticket_struct_t *ptr)
{
    // get the total tiks.
    ptr->common_total = es_count_common_tik();
    ptr->person_total = es_count_personailsed_tik();

    // check if we have at least one ticket...
    if (!ptr->common_total && !ptr->person_total)
    {
        print_message_loop_lock("no tickets!\n\n you have a very clean switch!\n");
        return false;
    }

    // setup common ticket data.
    if (ptr->common_total)
    {
        FsRightsId *rights_id = malloc(ptr->common_total * sizeof(FsRightsId));

        if (ptr->common_total != es_list_common_tik(rights_id, ptr->common_total))
        {
            free(rights_id);
            return false;
        }

        // size of data.
        ptr->common_tiks = malloc(ptr->common_total * sizeof(temp_t));
        memset(ptr->common_tiks, 0, ptr->common_total * sizeof(temp_t));

        for (u32 i = 0; i < ptr->common_total; i++)
        {
            memcpy(&ptr->common_tiks[i].ncm_rights.rights_id, &rights_id[i], sizeof(FsRightsId));
            ptr->common_tiks[i].ncm_rights.key_generation = get_key_gen_from_rights_id(ptr->common_tiks[i].ncm_rights.rights_id);
            ptr->common_tiks[i].app_id = get_app_id_from_rights_id(ptr->common_tiks[i].ncm_rights.rights_id);
        }
        
    }

    
    // setup person ticket data.
    if (ptr->person_total)
    {
        // size of data.
        ptr->person_tiks = malloc(ptr->person_total * sizeof(temp_t));
        memset(ptr->person_tiks, 0, ptr->person_total * sizeof(temp_t));


        if (ptr->person_total != es_list_personalised_tik(ptr->person_tiks, ptr->person_total))
        {
            return false;
        }
    }
    

    // return true if all went well...
    return true;
}


void ticket_menu_refresh_screen(u32 total, u32 list_move)
{
    for (u32 i = 0, j = list_move; i < 20; i++, j++)
    {
        ;
    }
}
*/

void ticket_menu()
{
    u32 common_total = es_count_common_tik();
    
    if (!common_total)
    {
        print_message_loop_lock("no tickets installed\n");
        return;
    }

    FsRightsId *rights_id = malloc(common_total * sizeof(FsRightsId));
    memset(rights_id, 0, common_total * sizeof(FsRightsId));

    if (R_FAILED(es_list_common_tik(rights_id, common_total)))
    {
        free(rights_id);
        return;
    }

    for (u32 i = 0; i < common_total; i++)
    {
        NacpLanguageEntry *lang_entry;
        nacp_get_lang_entry_from_rights_id(rights_id[i], &lang_entry);

        if (lang_entry == NULL)
            print_message_loop_lock("%lu\n\n", rights_id[i]);
        else
            print_message_loop_lock("name: %s\tauthor: %s\n\n", lang_entry->name, lang_entry->author);
    }

    free(rights_id);
}