#!/usr/bin/python
import usb1
import libnetmd

def main(bus=None, device_address=None):
    context = usb1.LibUSBContext()
    for md in libnetmd.iterdevices(context, bus=bus,
                                   device_address=device_address):
        writeMDTitle(md)

def writeMDTitle(md):
    md_iface = libnetmd.NetMDInterface(md)

    md_iface.cacheTOC()
    md_iface.setTrackTitle(2, "\x82\xcb\x82\xb1\x81\x40\x94\x4c", wchar=True)
    md_iface.syncTOC()

    md_iface.stop()

if __name__ == '__main__':
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option('-b', '--bus')
    parser.add_option('-d', '--device')
    (options, args) = parser.parse_args()
    assert len(args) == 0

    main(bus=options.bus, device_address=options.device)

