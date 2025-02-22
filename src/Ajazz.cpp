//
// Created by taxmachine on 16/02/25.
//

#include "Ajazz.hpp"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>
#include <unistd.h>

#include "utils.hpp"
#include "lodepng/lodepng.h"

#define HID_SET_REPORT 0x09

Ajazz::Ajazz(const uint16_t vid, const uint16_t pid) {
    hid_init();

    this->handle = hid_open(vid, pid, nullptr);
    if (!handle) {
        throw std::runtime_error("Couldn't open device: " + utils::wstr2str(hid_error(this->handle)));
    }

    this->vid = vid,
    this->pid = pid;
}

Ajazz::~Ajazz() {
    if (this->handle) {
        hid_close(this->handle);
    }

    hid_exit();
}

KeyboardInformation Ajazz::getHardwareInformation() const {
    wchar_t manufacturer[255];
    wchar_t product[255];
    wchar_t serialNumber[255];

    int result = hid_get_manufacturer_string(this->handle, manufacturer, 255);
    if (result < 0) {
        throw std::runtime_error("Couldn't get manufacturer string");
    }

    result = hid_get_product_string(this->handle, product, 255);
    if (result < 0) {
        throw std::runtime_error("Couldn't get manufacturer string");
    }

    result = hid_get_serial_number_string(this->handle, serialNumber, 255);
    if (result < 0) {
        throw std::runtime_error("Couldn't get manufacturer string");
    }

    return {
        .vendorID = this->vid,
        .productID = this->pid,
        .manufacturer = std::wstring{manufacturer},
        .product = std::wstring{product},
        .serialNumber = std::wstring{serialNumber},
    };
}


void Ajazz::sendInterruptPacket(uint8_t *packet, const size_t length) const {

}

void Ajazz::receiveInterruptPacket(uint8_t *packet, const size_t length) const {

}


void Ajazz::sendControlPacket(const uint8_t *packet, const size_t length, const int max_retries) const {
    for (int i = 0; i < max_retries; i++) {
        if (hid_send_feature_report(this->handle, packet, length) >= 0) return;
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
    throw std::runtime_error("Ajazz::sendControlPacket error: " + utils::wstr2str(hid_error(this->handle)));
}

void Ajazz::receiveControlPacket(uint8_t *packet, const size_t length, const int max_retries) const {
    for (int i = 0; i < max_retries; i++) {
        if (hid_get_feature_report(this->handle, packet, length) >= 0) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    //throw std::runtime_error("Ajazz::receiveControlPacket error: " + utils::wstr2str(hid_error(this->handle)));
}



// TODO reverse engineer custom RGB data
void Ajazz::setColor(const uint8_t r, const uint8_t g, const uint8_t b) const {

}

void Ajazz::setMode(const LightingMode mode, const uint8_t r, const uint8_t g, const uint8_t b,
                    const bool rainbow, const int brightness, const int speed, const DIRECTION direction) const {
    if (brightness > MAX_BRIGHTNESS) {
        throw std::runtime_error("Brightness level cannot be over 5");
    }

    if (speed > MAX_SPEED) {
        throw std::runtime_error("Speed cannot be over 5");
    }

    Packets::ModePacket packet{};
    packet.mode = static_cast<uint8_t>(mode);
    packet.r = r;
    packet.g = g;
    packet.b = b;
    packet.options.rainbow = rainbow;
    packet.options.brightness = brightness;
    packet.options.speed = speed;
    packet.options.direction = direction;
    packet.unknown1 = 0xaa;
    packet.unknown2 = 0x55;

    uint8_t data[PACKET_LENGTH];
    memcpy(data, &packet, PACKET_LENGTH);

    hid_set_nonblocking(this->handle, 0);

    auto* received = static_cast<uint8_t *>(malloc(PACKET_LENGTH));
    hid_send_feature_report(this->handle, Packets::START, PACKET_LENGTH);
    this->receiveControlPacket(received, RESPONSE_PACKET_LENGTH);

    hid_send_feature_report(this->handle, Packets::START_MODE, PACKET_LENGTH);
    this->receiveControlPacket(received, RESPONSE_PACKET_LENGTH);

    hid_send_feature_report(this->handle, data, PACKET_LENGTH);
    this->receiveControlPacket(received, RESPONSE_PACKET_LENGTH);

    hid_send_feature_report(this->handle, Packets::FINISH, PACKET_LENGTH);
    this->receiveControlPacket(received, RESPONSE_PACKET_LENGTH);

    free(received);
}

void Ajazz::uploadGIF(const std::string& path) const {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("This file does not exists");
    }

    std::vector<unsigned char> png;
    std::vector<unsigned char> image;
    unsigned width, height;

    unsigned error = lodepng::load_file(png, path);
    if (error) {
        throw std::runtime_error("lodepng::load_file error: " + std::string(lodepng_error_text(error)));
    }
    error = lodepng::decode(image, width, height, png);
    if (error) {
        throw std::runtime_error("lodepng::decode error: " + std::string(lodepng_error_text(error)));
    }

    utils::printHexData(image.data(), image.size());

    //this->sendControlPacket(Packets::START, PACKET_LENGTH);
    //this->sendControlPacket(Packets::START_IMAGE, PACKET_LENGTH);
    //this->sendInterruptPacket(data, IMAGE_CHUNK_SIZE);
    //this->sendControlPacket(Packets::FINISH, PACKET_LENGTH);
}
