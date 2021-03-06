#ifndef _NS_H_
#define _NS_H_

#include <switch.h>


// returns the total size of the sd card.
size_t ns_get_sd_storage_total_size();

// returns the free space on the sd card.
size_t ns_get_sd_storage_free_space();

// store data found to out.
// make sure to set the size of the out large enough.
// returns the total stored.
s32 ns_list_app_record(NsApplicationRecord *out, s32 count, s32 offset);

//
s32 ns_list_app_cnmt_status(NsApplicationContentMetaStatus *out, s32 count, u64 app_id);

// store data to out.
Result ns_get_app_control_data(NsApplicationControlData *out, u64 app_id);

//
s32 ns_get_app_delivery_info(NsApplicationDeliveryInfo *out, s32 count, u64 app_id, u32 attr);

//
bool ns_check_app_delivery_info(const NsApplicationDeliveryInfo *info);

//
s32 ns_compare_app_delivery_info(const NsApplicationDeliveryInfo *info0, const NsApplicationDeliveryInfo *info1);

//
bool ns_check_if_can_deliver_app_info(NsApplicationDeliveryInfo *info0, s32 count0, NsApplicationDeliveryInfo *info1);

//
s32 ns_list_content_meta_key(NcmContentMetaKey *meta, NsApplicationDeliveryInfo *info);


// seems to always return 4.
u32 ns_count_application_record(u64 app_id);

//
Result ns_delete_application_completely(u64 app_id);


// push an application record.
Result ns_push_application_record(u64 app_id, void *cnmt_storage_records, size_t data_size);

// delete an application record using the app_id.
Result ns_delete_application_record(u64 app_id);

// count the amount of content already installed.
u32 ns_count_application_content_meta(u64 app_id);

// write all existing content to void *out_buf. Call this after count_out > 1.
Result ns_list_application_record_content_meta(u64 offset, u64 app_id, void *out_buf, size_t out_buf_size, u32 count);

#endif