#ifndef _TICKET_H_
#define _TICKET_H_

#include <switch.h>


typedef enum
{
    common_ticket = 0,
    personalised_ticket = 1
} tik_type;

typedef struct
{
    char name[0x200];
    FsRightsId rights_id;   // maybe use ncmrights and then fetch key gen...
    tik_type type;          // see `tik_type`
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

#endif