#ifndef _ES_H_
#define _ES_H_

#include <switch.h>


// start es services.
Result es_start_service(void);

// close es services.
void es_close_service(void);

// import tik and cert.
Result es_import_tik_and_cert(void const *tik_buf, size_t tik_size, void const *cert_buf, size_t cert_size);

// delete a ticket using the given rights_id.
Result es_delete_common_tik(const FsRightsId *rights_id);
Result es_delete_personalised_tik(const FsRightsId *rights_id);
Result es_delete_all_common_tik(void);
Result es_delete_all_personalised_tik(void);

// return the num of tickets.
uint32_t es_count_common_tik(void);
uint32_t es_count_personailsed_tik(void);

// store all data to out.
// count is the the total number of tiks.
// has error checking if the given count does not match the total right_id obtained.
Result es_list_common_tik(FsRightsId *out, uint32_t count);
Result es_list_personalised_tik(FsRightsId *out, uint32_t count);

//
uint32_t es_get_common_tik_size(const FsRightsId *rights_id);
Result es_get_common_tik_data(void *out, size_t out_size, const FsRightsId *rights_id);

#endif