import pyshark
import json
import textwrap
from typing import List

LIGHTING_MODES = {
    0x00: "LED_OFF",
    0x01: "STATIC",
    0x02: "SINGLE_ON",
    0x03: "SINGLE_OFF",
    0x04: "GLITTERING",
    0x05: "FALLING",
    0x06: "COLOURFUL",
    0x07: "BREATH",
    0x08: "SPECTRUM",
    0x09: "OUTWARD",
    0x0a: "SCROLLING",
    0x0b: "ROLLING",
    0x0c: "ROTATING",
    0x0d: "EXPLODE",
    0x0e: "LAUNCH",
    0x0f: "RIPPLES",
    0x10: "FLOWING",
    0x11: "PULSATING",
    0x12: "TILT",
    0x13: "SHUTTLE"
}

DIRECTION = {
    0: "LEFT",
    1: "DOWN",
    2: "UP",
    3: "RIGHT"
}

def hexdump(src, length=16, sep='.'):
    FILTER = ''.join([(len(repr(chr(x))) == 3) and chr(x) or sep for x in range(256)])
    lines = []
    for c in range(0, len(src), length):
        chars = src[c: c + length]
        hex_ = ' '.join(['{:02x}'.format(x) for x in chars])
        if len(hex_) > 24:
            hex_ = '{} {}'.format(hex_[:24], hex_[24:])
        printable = ''.join(['{}'.format((x <= 127 and FILTER[x]) or sep) for x in chars])
        lines.append('{0:08x}  {1:{2}s} |{3:{4}s}|'.format(c, hex_, length * 3, printable, length))
    return lines

def encode(data: List[int]) -> List[str]:
    return ["0x" + hex(byte)[2:].zfill(2) for byte in data]

capture = pyshark.FileCapture("/home/taxmachine/Downloads/sleep.pcapng", include_raw=True, use_json=True)
f = open("output_direction.txt", "w")

for packet in capture:
    if packet.usb.endpoint_address == 0x80:
        continue

    packet_raw = packet.get_raw_packet()
    dump = '\n'.join(hexdump(packet_raw))
    if not '00000060' in dump:
        continue

    if len(packet_raw) != 100:
        continue

    report_id = packet_raw[36]
    command_id = packet_raw[37]

    if report_id == 0x4 and (command_id == 0x2 or
                             command_id == 0xf5 or
                             command_id == 0x18 or
                             command_id == 0x13 or
                             command_id == 0xf0):
        continue
    details = f"""
---------------------- MODE PACKET -----------------------
    Mode: {LIGHTING_MODES[report_id]} ({report_id})
    Color: R: {packet_raw[37]} G: {packet_raw[38]} B: {packet_raw[39]}
    Rainbow?: {packet_raw[44]}
    Brightness: {packet_raw[45]}
    Speed: {packet_raw[46]}
    Direction: {DIRECTION[packet_raw[47]]} ({packet_raw[47]})
"""
    f.write(details)
    f.write(dump + "\n\n")
    f.write("------------------------------------------------------------\n")

f.close()