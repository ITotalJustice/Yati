#ifndef _USB_H_
#define _USB_H_

#include <switch.h>

typedef struct
{
    u64 mode;
    u8 padding[0x7];
    u64 offset_start;
    u8 padding2[0x7];
    u64 data_size;
    u8 padding3[0x7];
} usb_struct_t;


// read into void *out return size of data read.
size_t usb_read(void *out, size_t len, u64 offset);

// write from void *in, return the size of the data written.
size_t usb_write(const void *in, size_t len);

// call this before every read.
void usb_poll(int mode, size_t offset, size_t data_size);

//
void usb_get_nsp(NcmStorageId storage_id);

// call this to exit usb comms.
void usb_exit(void);

#endif