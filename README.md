# Linux Software for the Ajazz
This is a software to control LEDs, lighting modes, LCD screen and macros

## TODO
 - [ ] Install and easy use
 - [x] Lighting modes
 - [ ] Individual LEDs
 - [ ] Macros
 - [x] LCD screen images & GIF
 - [x] MULTI LANGUAGE SUPPORT
 - [ ] Sound Reaction (??)
 - [ ] Battery Status

## SO tested
 - Arch Linux with GNOME
 - Arch Linux with KDE
 - Arch Linux with Hyprland

## Installation & Build
Usual cmake build. You need the following dependencies:
```bash
sudo apt install cmake g++ libhidapi-dev libgtk-3-dev libglfw3-dev libgl1-mesa-dev
```
Then build the project:
```bash
cmake -B build && cmake --build build -j$(nproc)
```

## Linux Device Permissions (udev rules)
To use the software without root (`sudo`) and enable RGB and TFT screen uploads for both **USB cable** and **2.4GHz wireless dongle**:

```bash
echo 'SUBSYSTEM=="hidraw", ATTRS{idVendor}=="0c45", MODE="0666"' | sudo tee /etc/udev/rules.d/99-ajazz-keyboard.rules
sudo udevadm control --reload-rules && sudo udevadm trigger
```
After applying the rule, unplug and replug the USB cable or 2.4GHz receiver.
> **Note on Screen Uploads:** TFT screen image and GIF transfers require the keyboard to be connected directly via USB cable.

## Credits & Modifications
- Original software by [TaxMachine](https://github.com/TaxMachine/ajazz-keyboard-software-linux).
- Modified and extended by **Vixcho** (October 2026):
  - 2.4GHz wireless dongle support and dual-interface communication
  - TFT Screen image and animated GIF uploading with live preview and progress tracking
  - Local screen gallery cache
  - Multi-language support (English default & Spanish)
  - Dedicated configuration settings tab with persistent storage
  - Per-key individual RGB lighting editor and macro management
  - Diagnostic logs viewer and universal udev permission tools
