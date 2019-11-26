#include <stdio.h>
#include <stdlib.h>
#include <switch.h>

#include "es.h"
#include "nca.h"
#include "usb.h"
#include "network.h"
#include "util.h"


// globals.
static Service g_es_service;


Result es_start_service()
{
    Result rc = 0;
    if (serviceIsActive(&g_es_service)) return rc;

    rc = smGetService(&g_es_service, "es");
    if (R_FAILED(rc))
        printf("failed to start es service\n");
    return rc;
}

void es_close_service()
{
    serviceClose(&g_es_service);
}

Result es_import_tik_and_cert(void const *tik_buf, size_t tik_size, void const *cert_buf, size_t cert_size)
{
    Result rc = serviceDispatch(&g_es_service, 1,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In, SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
        .buffers = { { tik_buf, tik_size }, { cert_buf, cert_size } });
    if (R_FAILED(rc))
        print_message_loop_lock("failed to import tik / cert\n");
    return rc;
}

Result es_delete_tik(const FsRightsId *rights_id, size_t size)
{
    Result rc = serviceDispatch(&g_es_service, 3,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
        .buffers = { { rights_id, size }, });
    if (R_FAILED(rc))
        print_message_loop_lock("failed to delete tik\n");
    return rc;
}

Result es_get_title_key(const FsRightsId *rights_id, u8 *out, size_t out_size)
{
    Result rc = serviceDispatchIn(&g_es_service, 8, *rights_id,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { out, out_size } });
    if (R_FAILED(rc))
        print_message_loop_lock("failed to get tik\n");
    return rc;
}

u32 es_count_common_tik(void)
{
    u32 out_total = 0;
    Result rc = serviceDispatchOut(&g_es_service, 9, out_total, SfOutHandleAttr_None);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to count common tickets\n");
    return out_total;
}

u32 es_count_personailsed_tik(void)
{
    u32 out_total = 0;
    Result rc = serviceDispatchOut(&g_es_service, 10, out_total, SfOutHandleAttr_None);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to count personalized tickets\n");
    return out_total;
}

u32 es_list_common_tik(FsRightsId *out, size_t out_size)
{
    u32 out_total = 0;
    u64 buffer_idk = 0;
    Result rc = serviceDispatchInOut(&g_es_service, 11, buffer_idk, out_total,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { out, out_size } });
    if (R_FAILED(rc))
        print_message_loop_lock("failed to list common tickets\n");
    return out_total;
}

u32 es_list_personalised_tik(FsRightsId *rights_id, size_t size)
{
    u32 out_total = 0;
    u64 buffer_idk = 0;
    Result rc = serviceDispatchInOut(&g_es_service, 12, buffer_idk, out_total,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { rights_id, size } });
    if (R_FAILED(rc))
        print_message_loop_lock("failed to list personalised tickets\n");
    return out_total;
}

Result es_get_common_tik_data(void *out, size_t out_size, const FsRightsId *rights_id)
{
    u64 buffer_idk = 0;
    Result rc = serviceDispatchInOut(&g_es_service, 16, rights_id, buffer_idk,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { out, out_size } });
    if (R_FAILED(rc))
        print_message_loop_lock("failed to get common ticket data\n");
    return rc;
}