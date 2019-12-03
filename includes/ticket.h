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
    u8 issuer[0x40];
    u8 title_key_block[0x100];
    u8 _0x140;                  //unkown.
    u8 title_key_type;          // see TicketType.
    u8 _0x142[0x3];             //unkown.
    u8 master_key_revision;
    u8 _0x146[0xA];             //unkown.
    u64 ticket_id;
    u64 device_id;
    FsRightsId rights_id;
    u32 account_id;
    u8 _0x174[0xC];             //unkown.
    u8 title_key_enc[0x10];
    u8 _0x190[0x130];           //unkown.
} ticket_data_t;

typedef struct
{
    char name[0x200];
    FsRightsId rights_id;   // maybe use ncmrights and then fetch key gen...
    TicketType type;        // see TicketType
} ticket_info_struct_t;

typedef struct
{
    u32 common_total;
    u32 personalised_total;
    u64 total;

    ticket_info_struct_t *info;    // see tik_info_struct_t.
} ticket_struct_t;


//
bool ticket_get_tik_total(ticket_struct_t *tik);

//
bool ticket_read_common(ticket_struct_t *tik);

//
bool ticket_read_personalised(ticket_struct_t *tik);

//
bool ticket_setup_tik_info(ticket_struct_t *tik);

// delete ticket.
Result ticket_delete(const FsRightsId *rights_id, TicketType type);

#endif