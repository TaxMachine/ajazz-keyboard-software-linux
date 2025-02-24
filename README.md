# Linux Software for the Ajazz SONiX AK820 Pro (VID 0x8009)
This is a software to control LEDs, lighting modes, LCD screen and macros

## TODO
 - [x] Lighting modes
 - [ ] Individual LEDs
 - [ ] Macros
 - [ ] LCD screen images & GIF
 - [ ] Sound Reaction (??)

## Installation
Usual cmake build. You also need to install [hidapi](https://github.com/libusb/hidapi)
```bash
sudo apt install libhidapi-dev
```

## How did I do it
At first, I thought it would be a good idea to reverse engineer the
official software but since it's made by chinese developers, even
the decompilation had weird control flow and logic so instead I used
Wireshark with PCapUSB to sniff packets and compare them
[I made a parser for pcap files that parses packets and show their infos](https://github.com/TaxMachine/ajazz-keyboard-software-linux/blob/master/analyze/hidfilter.py)
