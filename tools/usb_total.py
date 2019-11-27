import usb
import struct
import sys
import os
from pathlib import Path

nsp_name = ""


def read_nsp(range_size, range_offset, out_ep):
    with open(nsp_name, "rb") as f:
        f.seek(range_offset)

        curr_off = 0x0
        end_off = range_size
        read_size = 0x100000 # 1MiB

        while curr_off < end_off:
            if curr_off + read_size >= end_off:
                read_size = end_off - curr_off

            buf = f.read(read_size)
            out_ep.write(data=buf, timeout=0)
            curr_off += read_size
            print("reading {} - {}".format(curr_off, end_off))


def wait_for_input(in_ep, out_ep):
    print("now waiting for intput\n")

    while True:
        # read-in data sent from switch.
        file_range_header = in_ep.read(0x30, timeout=0)

        mode = struct.unpack('<Q', file_range_header[0:8])[0]
        range_offset = struct.unpack('<Q', file_range_header[16:24])[0]
        range_size = struct.unpack('<Q', file_range_header[32:40])[0]
        print("mode = {} | range offset = {} | range size = {}".format(mode, range_offset, range_size))

        if (mode == 0):
            break
        read_nsp(range_size, range_offset, out_ep)


if __name__ == '__main__':

    # check which mode the user has selected.
    args = len(sys.argv)
    if (args != 2):
        print("either run python usb_total.py game.nsp OR drag and drop the game onto the python file (if python is in your path)")
        sys.exit(1)

    nsp_name = Path(sys.argv[1])

    if not nsp_name.is_file():
        raise ValueError('must be a file!!!!!')

    # Find the switch
    print("waiting for to find the switch...\n")
    dev = usb.core.find(idVendor=0x057E, idProduct=0x3000)

    while (dev is None):
        dev = usb.core.find(idVendor=0x057E, idProduct=0x3000)

    print("found the switch!\n")

    dev.reset()
    dev.set_configuration()
    cfg = dev.get_active_configuration()

    is_out_ep = lambda ep: usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_OUT
    is_in_ep = lambda ep: usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_IN
    out_ep = usb.util.find_descriptor(cfg[(0,0)], custom_match=is_out_ep)
    in_ep = usb.util.find_descriptor(cfg[(0,0)], custom_match=is_in_ep)

    assert out_ep is not None
    assert in_ep is not None

    wait_for_input(in_ep, out_ep)