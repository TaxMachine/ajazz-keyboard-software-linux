//
// Created by taxmachine on 16/02/25.
//

#include "ak820pro.hpp"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>
#include <lodepng.h>

#include "../utils/utils.hpp"

#define HID_SET_REPORT 0x09

static uint8_t START[PACKET_LENGTH] = {COMMAND_PREFIX, START_COMMAND, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static uint8_t FINISH[PACKET_LENGTH] = {COMMAND_PREFIX, FINISH_COMMAND, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static uint8_t START_MODE[PACKET_LENGTH] = {COMMAND_PREFIX, MODE_COMMAND, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static uint8_t START_SLEEP[PACKET_LENGTH] = {COMMAND_PREFIX, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static uint8_t START_IMAGE[PACKET_LENGTH] = {COMMAND_PREFIX, IMAGE_COMMAND, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09};

AK820Pro::AK820Pro(const uint16_t vid, const uint16_t pid) {
    hid_init();

    this->vid = vid,
    this->pid = pid;
}

AK820Pro::~AK820Pro() {
    if (this->handle) {
        hid_close(this->handle);
    }

    hid_exit();
}

void AK820Pro::openHandle() {
    if (this->handle) {
        return;
    }

    this->handle = hid_open(this->vid, this->pid, nullptr);
    if (!this->handle) {
        throw std::runtime_error("Couldn't open handle to keyboard");
    }
}

void AK820Pro::closeHandle() {
    if (!this->handle) {
        return;
    }

    hid_close(this->handle);
    this->handle = nullptr;
}

void AK820Pro::executeHIDCommand(const std::vector<uint8_t*>& commands) const {
    if (!handle) {
        throw std::runtime_error("invalid hid handle");
    }

    hid_set_nonblocking(handle, 0);
    auto* received = static_cast<uint8_t*>(malloc(PACKET_LENGTH));

    for (size_t i = 0; i < commands.size(); i++) {
        hid_send_feature_report(handle, commands[i], PACKET_LENGTH);
        hid_get_feature_report(handle, received, RESPONSE_PACKET_LENGTH);
    }

    free(received);
}

// TODO reverse engineer custom RGB data
void AK820Pro::setColor(const uint8_t r, const uint8_t g, const uint8_t b) const {

}

void AK820Pro::setMode(const LightingMode mode, const uint8_t r, const uint8_t g, const uint8_t b,
                    const bool rainbow, const int brightness, const int speed, const Direction direction) const {
    if (!this->handle) {
        throw std::runtime_error("invalid hid handle");
    }


    if (brightness < 0 || brightness > MAX_BRIGHTNESS) {
        throw std::runtime_error("Brightness level cannot be over 5 or under 0");
    }

    if (speed < 0 || speed > MAX_SPEED) {
        throw std::runtime_error("Speed cannot be over 5 or under 0");
    }

    ModePacket packet{};
    packet.mode = static_cast<uint8_t>(mode);
    packet.r = r;
    packet.g = g;
    packet.b = b;
    packet.options.rainbow = rainbow;
    packet.options.brightness = brightness;
    packet.options.speed = speed;
    packet.options.direction = direction;
    packet.delimiter = 0xaa55;

    uint8_t data[PACKET_LENGTH];
    memcpy(data, &packet, PACKET_LENGTH);

    hid_set_nonblocking(this->handle, 0);

    auto* received = static_cast<uint8_t *>(malloc(PACKET_LENGTH));
    hid_send_feature_report(this->handle, START, PACKET_LENGTH);
    hid_get_feature_report(this->handle, received, RESPONSE_PACKET_LENGTH);

    hid_send_feature_report(this->handle, START_MODE, PACKET_LENGTH);
    hid_get_feature_report(this->handle, received, RESPONSE_PACKET_LENGTH);

    hid_send_feature_report(this->handle, data, PACKET_LENGTH);
    hid_get_feature_report(this->handle, received, RESPONSE_PACKET_LENGTH);

    hid_send_feature_report(this->handle, FINISH, PACKET_LENGTH);
    hid_get_feature_report(this->handle, received, RESPONSE_PACKET_LENGTH);

    free(received);
}

std::future<void> AK820Pro::setModeAsync(const LightingMode mode, const uint8_t r, const uint8_t g, const uint8_t b,
                                   const bool rainbow, const int brightness, const int speed, const Direction direction) const {
    return std::async(std::launch::async, [=] {
            if (brightness < 0 || brightness > MAX_BRIGHTNESS) {
                throw std::runtime_error("Brightness level cannot be over 5 or under 0");
            }

            if (speed < 0 || speed > MAX_SPEED) {
                throw std::runtime_error("Speed cannot be over 5 or under 0");
            }

            ModePacket packet{};
            packet.mode = static_cast<uint8_t>(mode);
            packet.r = r;
            packet.g = g;
            packet.b = b;
            packet.options.rainbow = rainbow;
            packet.options.brightness = brightness;
            packet.options.speed = speed;
            packet.options.direction = direction;
            packet.delimiter = 0xaa55;

            uint8_t data[PACKET_LENGTH];
            memcpy(data, &packet, PACKET_LENGTH);

            const std::vector commands = {START, START_MODE, data, FINISH};

            executeHIDCommand(commands);
        });
}

void AK820Pro::setSleepTime(const LightSleepTime sleep_time) const {
    uint8_t data[PACKET_LENGTH];
    data[8] = static_cast<uint8_t>(sleep_time);
    data[PACKET_LENGTH - 1] = 0x55;
    data[PACKET_LENGTH - 2] = 0xaa;

    hid_set_nonblocking(this->handle, 0);

    auto* received = static_cast<uint8_t *>(malloc(PACKET_LENGTH));
    hid_send_feature_report(this->handle, START, PACKET_LENGTH);
    hid_get_feature_report(this->handle, received, RESPONSE_PACKET_LENGTH);

    hid_send_feature_report(this->handle, START_SLEEP, PACKET_LENGTH);
    hid_get_feature_report(this->handle, received, RESPONSE_PACKET_LENGTH);

    hid_send_feature_report(this->handle, data, PACKET_LENGTH);
    hid_get_feature_report(this->handle, received, RESPONSE_PACKET_LENGTH);

    free(received);
}

std::future<void> AK820Pro::setSleepTimeAsync(const LightSleepTime sleep_time) const {
    return std::async(std::launch::async, [=] {
        uint8_t data[PACKET_LENGTH];
        data[8] = static_cast<uint8_t>(sleep_time);
        data[PACKET_LENGTH - 1] = 0x55;
        data[PACKET_LENGTH - 2] = 0xaa;

        const std::vector commands = {START, START_SLEEP, data};

        executeHIDCommand(commands);
    });
}

void AK820Pro::uploadGIF(const std::string& path) const {
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
