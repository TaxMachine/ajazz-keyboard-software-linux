//
// Created by taxmachine on 21/02/25.
//

#ifndef KEYBOARDDEFS_HPP
#define KEYBOARDDEFS_HPP

#include <hidapi/hidapi.h>

constexpr uint16_t AJAZZ_VID = 0x0c45;
constexpr uint16_t AK820PRO_PID = 0x8009;

constexpr uint8_t COMMAND_PREFIX = 0x04;
constexpr uint8_t MODE_COMMAND = 0x13;
constexpr uint8_t IMAGE_COMMAND = 0x72;
constexpr uint8_t START_COMMAND = 0x18;
constexpr uint8_t FINISH_COMMAND = 0xf0;

constexpr uint8_t PACKET_LENGTH = 64;
constexpr uint8_t RESPONSE_PACKET_LENGTH = 0;
constexpr uint16_t IMAGE_CHUNK_SIZE = 4123;

constexpr uint8_t MAX_BRIGHTNESS = 5;
constexpr uint8_t MAX_SPEED = 5;

struct KeyboardInformation {
    std::wstring manufacturer;
    std::wstring product;
    std::wstring serialNumber;
};

static KeyboardInformation getHardwareInformation(hid_device *handle) {
    wchar_t manufacturer[255];
    wchar_t product[255];
    wchar_t serialNumber[255];

    int result = hid_get_manufacturer_string(handle, manufacturer, 255);
    if (result < 0) {
        throw std::runtime_error("Couldn't get manufacturer string");
    }

    result = hid_get_product_string(handle, product, 255);
    if (result < 0) {
        throw std::runtime_error("Couldn't get manufacturer string");
    }

    result = hid_get_serial_number_string(handle, serialNumber, 255);
    if (result < 0) {
        throw std::runtime_error("Couldn't get manufacturer string");
    }

    return {
        .manufacturer = std::wstring{manufacturer},
        .product = std::wstring{product},
        .serialNumber = std::wstring{serialNumber},
    };
}

#endif //KEYBOARDDEFS_HPP
