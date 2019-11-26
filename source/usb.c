#include <stdio.h>
#include <stdlib.h>
#include <switch.h>

#include "util.h"
#include "nsp.h"
#include "xci.h"
#include "usb.h"


size_t usb_read(void *out, size_t len, u64 offset)
{
    usb_poll(1, offset, len);

    u8 *bufptr = (u8 *)out;
    size_t sizeRemaining = len;
    size_t tmpsize = 0;

    while (sizeRemaining)
    {
        tmpsize = usbCommsRead(bufptr, sizeRemaining);
        bufptr += tmpsize;
        sizeRemaining -= tmpsize;
    }
    return len;
}

size_t usb_write(const void* in, size_t len)
{
    const u8 *bufptr = (const u8 *)in;
    size_t sizeRemaining = len;
    size_t tmpsize = 0;

    while (sizeRemaining)
    {
        tmpsize = usbCommsWrite(bufptr, sizeRemaining);
        bufptr += tmpsize;
        sizeRemaining -= tmpsize;
    }
    return len;
}

void usb_poll(int mode, size_t offset, size_t data_size)
{
    usb_struct_t usb_struct = { mode, {0}, offset, {0}, data_size, {0} };
    usb_write(&usb_struct, sizeof(usb_struct_t));
}

void usb_exit()
{
    usb_poll(0, 0, 0);
}