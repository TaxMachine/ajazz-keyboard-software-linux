//
// Created by taxmachine on 16/02/25.
//

#include "Ajazz.hpp"

#include <cstring>
#include <iostream>

Ajazz::Ajazz(const uint16_t vid, const uint16_t pid) {
    int result = libusb_init(&this->ctx);
    if (result < 0) {
        throw std::runtime_error("libusb_init: " + getUsbError(result));
    }

    this->handle = libusb_open_device_with_vid_pid(ctx, vid, pid);
    if (this->handle == nullptr) {
        throw std::runtime_error("libusb_open_device_with_vid_pid: failed");
    }
}

Ajazz::~Ajazz() {
    if (this->handle) {
        libusb_close(this->handle);
    }

    if (this->ctx) {
        libusb_exit(ctx);
    }
}

std::string Ajazz::getUsbError(const int errorCode) {
    const std::string errorName(libusb_error_name(errorCode));
    const std::string error(libusb_strerror(errorCode));
    return errorName + " - " + error;
}

void Ajazz::printHexData(const unsigned char *data, int length) {
    std::cout << "Data: ";
    for (int i = 0; i < length; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

/*void Ajazz::sendPacket(uint8_t *packet, const size_t length) const {
    std::cout << "Sending mode command:" << std::endl;
    printHexData(packet, PACKET_LENGTH);

    int transferred;
    int result = libusb_interrupt_transfer(this->handle,
                              0x00,
                              packet,
                              static_cast<int>(length),
                              &transferred,
                              5000);

    if (result < 0) {
        throw std::runtime_error("libusb_interrupt_transfer (OUT): " + getUsbError(result));
    }

    uint8_t data[PACKET_LENGTH];
    int received;
    result = libusb_interrupt_transfer(this->handle,
                              0x80,
                              data,
                              static_cast<int>(length),
                              &received,
                              5000);

    if (result < 0) {
        throw std::runtime_error("libusb_interrupt_transfer (IN): " + getUsbError(result));
    }

    std::cout << "Received data" << std::endl;
    printHexData(data, received);
}*/

void Ajazz::sendInterruptPacket(uint8_t *packet, const size_t length) const {
    constexpr unsigned char endpoint = 0x03;
    int actual_length;
    const int result = libusb_interrupt_transfer(
        this->handle,
        endpoint,
        packet,
        length,
        &actual_length,
        5000);

    if (result < 0) {
        throw std::runtime_error("Interrupt transfer error: " + getUsbError(result));
    }
}

void Ajazz::sendControlPacket(uint8_t *packet, const size_t length) const {
    const int result = libusb_control_transfer(
        this->handle,
        0x21,
        0x09,
        0x0300,
        3,
        packet,
        length,
        5000);

    if (result < 0) {
        throw std::runtime_error("Control transfer error: " + getUsbError(result));
    }
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
    packet.mode = mode;
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

    this->sendControlPacket(Packets::START, PACKET_LENGTH);
    this->sendControlPacket(Packets::START_MODE, PACKET_LENGTH);
    this->sendControlPacket(data, PACKET_LENGTH);
    this->sendControlPacket(Packets::FINISH, PACKET_LENGTH);
}

void Ajazz::uploadGIF(const std::string& path) const {

    uint8_t* data = Packets::START_IMAGE_DATA;

    this->sendControlPacket(Packets::START, PACKET_LENGTH);
    this->sendControlPacket(Packets::START_IMAGE, PACKET_LENGTH);
    this->sendInterruptPacket(data, IMAGE_CHUNK_SIZE);
    this->sendControlPacket(Packets::FINISH, PACKET_LENGTH);
}
