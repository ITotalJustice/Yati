// TODO:
/*
    lots of code to be done...
    i want to make this a giant wrapper for nacp.
    dumping logo, storing logo data to a buffer (for gui later).
    quick access to name / author.
    quick access to age rating, version, supportedLanguages.
    function to ZERO `requiredNetworkServiceLicenseOnLaunch` for certian games.

*/

#include <stdio.h>
#include <switch.h>

#include "fs.h"
#include "ns.h"
#include "util.h"


Result nacp_get_lang_entry(NacpStruct *nacp, NacpLanguageEntry **out)
{
    Result rc = nacpGetLanguageEntry(nacp, out);
    if (R_FAILED(rc))
        print_message_loop_lock("failed to get lang entry\n");
    if (*out == NULL)
        print_message_loop_lock("lang entry is NULL\n");
    return rc;
}

Result nacp_get_lang_entry_from_app_id(u64 app_id, NacpLanguageEntry **out)
{
    NsApplicationControlData data;
    Result rc = ns_get_app_control_data(&data, app_id);
    if (R_FAILED(rc))
        return rc;
    return nacp_get_lang_entry(&data.nacp, out);
}

Result nacp_get_lang_entry_from_rights_id(FsRightsId rights_id, NacpLanguageEntry **out)
{
    u64 app_id = fs_get_app_id_from_rights_id(rights_id);
    return nacp_get_lang_entry_from_app_id(app_id, out);
}

void nacp_zero_nnid_required_bit(NacpStruct *nacp)
{
    // check against database to see if game should be set to zero.
    // return error codes based on response.
    // have this function callable by the user
    nacp->requiredNetworkServiceLicenseOnLaunch = 0;
}