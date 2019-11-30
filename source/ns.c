#include <stdio.h>
#include <switch.h>

#include "ns.h"
#include "util.h"


// global services.
static Service g_ns_service;
static Service g_test_service;
static int service_state = 0;


size_t ns_get_storage_total_size(NcmStorageId storage_id)
{
    size_t size = 0;
    if (R_FAILED(nsGetTotalSpaceSize(storage_id, &size)))
        print_message_loop_lock("failed to get ns total storage size\n");
    return size;
}

size_t ns_get_storage_free_space(NcmStorageId storage_id)
{
    size_t size = 0;
    if (R_FAILED(nsGetFreeSpaceSize(storage_id, &size)))
        print_message_loop_lock("failed to get ns total free space\n");
    return size;
}

int ns_list_app_record(NsApplicationRecord *out, s32 count, s32 offset)
{
    int out_count = 0;
    if (R_FAILED(nsListApplicationRecord(out, count, offset, &out_count)))
        print_message_loop_lock("failed to list app records\n");
    return out_count;
}

int ns_list_app_cnmt_status(NsApplicationContentMetaStatus *out, s32 count, u64 app_id)
{
    int out_count = 0;
    if (R_FAILED(nsListApplicationContentMetaStatus(app_id, 0, out, count, &out_count)))
        print_message_loop_lock("failed to list cnmt status\n");
    return out_count;
}

Result ns_get_app_control_data(NsApplicationControlData *out, u64 app_id)
{
    size_t out_size = 0;
    Result rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, app_id, out, sizeof(NsApplicationControlData), &out_size);
    //if (R_FAILED(rc))
        //print_message_loop_lock("failed to get app control data with app_id: %lu\n", app_id);
    return rc;
}

int ns_get_app_delivery_info(NsApplicationDeliveryInfo *out, s32 count, u64 app_id, u32 attr)
{
    int out_count = 0;
    if (!hosversionAtLeast(4, 0, 0))
        return out_count;
    if (R_FAILED(nsGetApplicationDeliveryInfo(out, count, app_id, attr, &out_count)))
        print_message_loop_lock("failed to get app delivery info\n");
    return out_count;
}

bool ns_check_app_delivery_info(const NsApplicationDeliveryInfo *info)
{
    bool res = false;
    if (!hosversionAtLeast(4, 0, 0))
        return res;
    if (R_FAILED(nsHasAllContentsToDeliver(info, 1, &res)))
        print_message_loop_lock("failed to check for valid app delivery info\n");
    return res;
}

int ns_compare_app_delivery_info(const NsApplicationDeliveryInfo *info0, const NsApplicationDeliveryInfo *info1)
{
    int res = -1;
    if (!hosversionAtLeast(4, 0, 0))
        return res;
    if (R_FAILED(nsCompareApplicationDeliveryInfo(info0, 1, info1, 1, &res)))
        print_message_loop_lock("failed to compare app delivery infos\n");
    return res;
}

bool ns_check_if_can_deliver_app_info(NsApplicationDeliveryInfo *info0, s32 count0, NsApplicationDeliveryInfo *info1)
{
    bool res = false;
    if (!hosversionAtLeast(4, 0, 0))
        return res;
    if (R_FAILED(nsCanDeliverApplication(info0, count0, info1, 1, &res)))
        print_message_loop_lock("failed to check if app info can be delivered\n");
    return res;
}

int ns_list_content_meta_key(NcmContentMetaKey *meta, NsApplicationDeliveryInfo *info)
{
    int total_out = 0;
    if (!hosversionAtLeast(4, 0, 0))
        return total_out;
    if (R_FAILED(nsListContentMetaKeyToDeliverApplication(meta, 1, 0, info, 1, &total_out)))
        print_message_loop_lock("failed to list content meta key\n");
    return total_out;
}

/*void mm()
{
    nsNeedsSystemUpdateToDeliverApplication();
}

void mm2()
{
    nsEstimateRequiredSize();
}

void mm3()
{
    nsRequestReceiveApplication();
}

void mm4()
{
    nsCommitReceiveApplication();
}

void mm5()
{
    nsGetReceiveApplicationProgress();
}

void mm6()
{
    nsListNotCommittedContentMeta();
}*/



// from ogfoil, slightly modified.
Result ns_start_services()
{
    Result rc = 0;

    if (serviceIsActive(&g_ns_service) || serviceIsActive(&g_test_service))
        return rc;

    service_state = 1;

    if (R_FAILED(rc = smGetService(&g_ns_service, "ns:am2")))
    {
        print_message_loop_lock("failed to start ns:am2 service\n");
        return rc;
    }

    if (R_FAILED(rc = serviceDispatch(&g_ns_service, 7996, .out_num_objects = 1, .out_objects = &g_test_service)))
    {
        print_message_loop_lock("failed to dispatch a service\n");
        ns_close_services();
    }
        
    return rc;
}

void ns_close_services()
{
    if (!service_state) return;
    serviceClose(&g_test_service);
    serviceClose(&g_ns_service);
}

Result ns_push_application_record(u64 app_id, void *cnmt_storage_records, size_t data_size)
{
    struct
    {
        u8 last_modified_event;
        u8 padding[0x7];
        u64 app_id;
    } in = { 0x3, {0}, app_id };
    
    Result rc =  serviceDispatchIn(&g_test_service, 16, in,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
        .buffers = { { cnmt_storage_records, data_size } });

    if (R_FAILED(rc))
        print_message_loop_lock("failed to push application record\n");
    return rc;
}

Result ns_delete_application_record(u64 app_id)
{
    Result rc = serviceDispatchIn(&g_test_service, 27, app_id, SfOutHandleAttr_None);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to delete application record\n");
    return rc;
}

u32 ns_count_application_content_meta(u64 app_id)
{
    u32 count = 0;
    Result rc = serviceDispatchInOut(&g_test_service, 600, app_id, count, SfOutHandleAttr_None);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to count app cnmt\n");
    return count;
}

Result ns_list_application_record_content_meta(u64 offset, u64 app_id, void *out_buf, size_t out_buf_size, u32 count)
{
    struct
    {
        u64 offset;
        u64 app_id;
    } in = { offset, app_id };
    u32 out = 0;

    Result rc = serviceDispatchInOut(&g_test_service, 17, in, out,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { out_buf, out_buf_size } });

    if (R_FAILED(rc))
        print_message_loop_lock("failed to list app cnmt\n");
    if (count != out)
        print_message_loop_lock("count difference\n");
    return rc;
}