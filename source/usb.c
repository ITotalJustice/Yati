#include <stdint.h>
#include <switch/services/usb.h>

#include "usb.h"


size_t usb_read(void *out, size_t len, uint64_t offset)
{
    usb_poll(1, offset, len);
    return usbCommsRead(out, len);
}

size_t usb_write(const void *in, size_t len)
{
    return usbCommsWrite(in, len);
}

void usb_poll(int mode, size_t offset, size_t data_size)
{
    usb_struct_t usb_struct = { mode, {0}, offset, {0}, data_size, {0} };
    usb_write(&usb_struct, sizeof(usb_struct_t));
}

void usb_exit(void)
{
    usb_poll(0, 0, 0);
}