//
// Created by taxmachine on 21/02/25.
// Modified by Vixcho on 13/10/26.
//

#ifndef KEYBOARDDEFS_HPP
#define KEYBOARDDEFS_HPP

#include <cstdint>
#include <hidapi/hidapi.h>
#include <stdexcept>
#include <string>

constexpr uint16_t AJAZZ_VID = 0x0c45;
constexpr uint16_t AK820PRO_PID = 0x8009;
constexpr uint16_t AK820PRO_PID_ALT = 0x800a;
constexpr uint16_t AK820PRO_PID_WIRELESS = 0xfdfd;

constexpr uint8_t COMMAND_PREFIX = 0x04;
constexpr uint8_t SAVE_COMMAND = 0x02;
constexpr uint8_t READ_ID_COMMAND = 0x05;
constexpr uint8_t READ_LIGHTING_COMMAND = 0x12;
constexpr uint8_t MODE_COMMAND = 0x13;
constexpr uint8_t START_COMMAND = 0x18;
constexpr uint8_t CUSTOM_LIGHT_COMMAND = 0x23;
constexpr uint8_t TIME_COMMAND = 0x28;
constexpr uint8_t IMAGE_COMMAND = 0x72;
constexpr uint8_t CUSTOM_MODE_VALUE = 0x80;
constexpr uint8_t FINISH_COMMAND = 0xf0;
constexpr uint8_t READ_PERKEY_COMMAND = 0xf5;

constexpr uint8_t PACKET_LENGTH = 64;
constexpr uint8_t RESPONSE_PACKET_LENGTH = 64;
constexpr uint16_t IMAGE_CHUNK_SIZE = 4096;
constexpr uint32_t TFT_WIDTH = 128;
constexpr uint32_t TFT_HEIGHT = 128;
constexpr uint32_t TFT_FRAME_BYTES = TFT_WIDTH * TFT_HEIGHT * 2;
constexpr uint32_t TFT_HEADER_BYTES = 256;

constexpr uint8_t MAX_BRIGHTNESS = 5;
constexpr uint8_t MAX_SPEED = 5;

struct KeyboardInformation {
  std::wstring manufacturer;
  std::wstring product;
  std::wstring serialNumber;
  bool isWireless = false;
  std::string devicePath;
};

static KeyboardInformation getHardwareInformation(hid_device *handle) {
  wchar_t manufacturer[255] = {0};
  wchar_t product[255] = {0};
  wchar_t serialNumber[255] = {0};

  hid_get_manufacturer_string(handle, manufacturer, 255);
  hid_get_product_string(handle, product, 255);
  hid_get_serial_number_string(handle, serialNumber, 255);

  return {.manufacturer = std::wstring{manufacturer},
          .product = std::wstring{product},
          .serialNumber = std::wstring{serialNumber},
          .isWireless = false,
          .devicePath = ""};
}

#endif // KEYBOARDDEFS_HPP
