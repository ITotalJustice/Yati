#ifndef _ES_H_
#define _ES_H_

#include <switch.h>


/*
* code from og foil, slightly modified.
*/


// start es services.
Result es_start_service(void);

// close es services.
void es_close_service(void);

//
// import tik and cert.
Result es_import_tik_and_cert(void const *tik_buf, size_t tik_size, void const *cert_buf, size_t cert_size);

//
Result es_delete_tik(const FsRightsId *rights_id, size_t size);

//
Result es_get_title_key(const FsRightsId *rights_id, u8 *out, size_t out_size);

//
u32 es_count_common_tik(void);

//
u32 es_count_personailsed_tik(void);

//
u32 es_list_common_tik(FsRightsId *out, size_t out_size);

//
u32 es_list_personalised_tik(FsRightsId *rights_id, size_t size);

//
Result es_get_common_tik_data(void *out, size_t out_size, const FsRightsId *rights_id);

#endif