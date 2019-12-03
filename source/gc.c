// TODO:
/*
    have menu_gc call functions from here.
        menu_gc should just list the cart installed.
            when gui is done, it will list icon, name author, fw version in update etc.
    have functions for
        normal dumping (maybe, probably no point having this in a title installer).
            or logo is fw4+
                i think i check update partition in gc to find that out??? or read from gc (xci) header somehow.
        secure dumping.
        update dumping.

        secure installing
            done: in menu_gc.c
            can contain multiple cnmt.nca, make function for both xci and gc to list all cnmt with file.
                can also be used with .nsp i guess for custom .nsp.
        
        update installing
            very far away from being done (not started fw installs!)
                will call install fw update, similar to how im just reading the ncas in secure the calling nca install...

        cert dumping (maybe, probably no point having this in a title installer).
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "fs.h"


typedef struct
{
    FsDeviceOperator d_op;
    FsGameCardHandle gc_handle;
    FsFileSystem gc_system;
} gc_struct_t;


Result gc_setup_mount(gc_struct_t *gc_struct, FsGameCardPartition partition)
{
    Result rc = 0;
    if (R_SUCCEEDED(rc = fs_open_device_operator(&gc_struct->d_op)))
    {
        if (fs_is_game_card_inserted(&gc_struct->d_op))
        {
            if (R_SUCCEEDED(rc = fs_get_game_card_handle(&gc_struct->d_op, &gc_struct->gc_handle)))
            {

            }
        }
        fs_close_device_operator(&gc_struct->d_op);
    }
    return rc;
}

Result gc_mount_system_partition(gc_struct_t *gc, FsGameCardPartition partition)
{
    Result rc = 0;
    rc = fs_open_device_operator(&gc->d_op);
    if (R_FAILED(rc))
        return rc;
    
    rc = fs_get_game_card_handle(&gc->d_op, &gc->gc_handle);
    if (R_FAILED(rc))
    {
        fs_close_device_operator(&gc->d_op);
        return rc;
    }

    rc = fs_open_game_card(&gc->gc_handle, partition, &gc->gc_system);
    if (R_FAILED(rc))
    {
        fs_close_device_operator(&gc->d_op);
        return rc;
    }

    return rc;
}

Result gc_dump_update()
{
    

}

Result gc_dump_normal()
{

}

Result gc_dump_secure(void)
{
    Result rc = 0;
    gc_struct_t gc;
    rc = gc_mount_system_partition(&gc, FsGameCardPartition_Secure);

    // get name of game from gc.
    // create dir with title name.

    // for now, just create dump folder and write ncas to that folder.
    
    return rc;
}
