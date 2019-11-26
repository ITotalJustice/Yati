#ifndef _NS_H_
#define _NS_H_

#include <switch.h>


//
size_t ns_get_storage_total_size(NcmStorageId storage_id);

//
size_t ns_get_storage_free_space(NcmStorageId storage_id);

// store data found to out.
// make sure to set the size of the out large enough.
// returns the total stored.
int ns_list_app_record(NsApplicationRecord *out, s32 count, s32 offset);

//
int ns_list_app_cnmt_status(NsApplicationContentMetaStatus *out, s32 count, u64 app_id);

// store data to out.
// pass in the size of the data.
// returns output size.
size_t ns_get_app_control_data(NsApplicationControlData *out, size_t size, u64 app_id);

//
int ns_get_app_delivery_info(NsApplicationDeliveryInfo *out, s32 count, u64 app_id, u32 attr);

//
bool ns_check_app_delivery_info(const NsApplicationDeliveryInfo *info);

//
int ns_compare_app_delivery_info(const NsApplicationDeliveryInfo *info0, const NsApplicationDeliveryInfo *info1);

//
bool ns_check_if_can_deliver_app_info(NsApplicationDeliveryInfo *info0, s32 count0, NsApplicationDeliveryInfo *info1);

//
int ns_list_content_meta_key(NcmContentMetaKey *meta, NsApplicationDeliveryInfo *info);


/*
* code from og foil, slightly modified.
*/

// start services ns:h2 and es.
Result ns_start_services(void);

// close services ns:h2 and es.
void ns_close_services(void);

// push an application record.
Result ns_push_application_record(u64 title_id, void *cnmt_storage_records, size_t data_size);

// delete an application record using the title_id.
Result ns_delete_application_record(u64 title_id);

// count the amount of content already installed.
u32 ns_count_application_content_meta(u64 title_id);

// write all existing content to void *out_buf. Call this after count_out > 1.
Result ns_list_application_record_content_meta(u64 offset, u64 title_id, void *out_buf, size_t out_buf_size, u32 count);

Result ns_launch_app(u64 title_id);

#endif