#ifndef _NACP_H_
#define _NACP_H_

#include <stdint.h>
#include <switch.h>


//
Result nacp_get_lang_entry(NacpStruct *nacp, NacpLanguageEntry **out);

//
Result nacp_get_lang_entry_from_app_id(uint64_t app_id, NacpLanguageEntry **out);

// 
Result nacp_get_lang_entry_from_rights_id(FsRightsId rights_id, NacpLanguageEntry **out);


#endif