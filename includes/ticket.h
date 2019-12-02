#ifndef _TICKET_H_
#define _TICKET_H_

#include <switch.h>


typedef enum
{
    TicketType_Common = 0,
    TicketType_Personalised = 1
} TicketType;

typedef struct
{
    char name[0x200];
    FsRightsId rights_id;   // maybe use ncmrights and then fetch key gen...
    TicketType type;        // see TicketType
} tik_info_struct_t;

typedef struct
{
    u32 common_total;
    u32 personalised_total;
    u64 total;

    tik_info_struct_t *info;    // see `tik_info_struct_t`.
} tik_struct_t;


//
bool ticket_get_tik_total(tik_struct_t *tik);

//
bool ticket_read_common(tik_struct_t *tik);

//
bool ticket_read_personalised(tik_struct_t *tik);

//
bool ticket_setup_tik_info(tik_struct_t *tik);

// delete ticket.
Result ticket_delete(const FsRightsId *rights_id, TicketType type);

#endif