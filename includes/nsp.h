#ifndef _NSP_H_
#define _NSP_H_

#include <switch.h>
#include "pfs0.h"
#include "util.h"


// this should be called after nsp_setup_install.
// this starts the install of the cnmt.
// if sucessful, it will install all ncas and tik / cert (if found).
void nsp_start_install(pfs0_struct_ptr *ptr, NcmStorageId storage_id, InstallProtocal mode);

// setup nsp install.
// parses the pfs0.
void nsp_setup_install(const char *file_name, NcmStorageId storage_id, InstallProtocal mode);

#endif