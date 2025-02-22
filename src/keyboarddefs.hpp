//
// Created by taxmachine on 21/02/25.
//

#ifndef KEYBOARDDEFS_HPP
#define KEYBOARDDEFS_HPP

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
    uint16_t vendorID;
    uint16_t productID;
    std::wstring manufacturer;
    std::wstring product;
    std::wstring serialNumber;
};

#endif //KEYBOARDDEFS_HPP
