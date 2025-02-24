//
// Created by taxmachine on 16/02/25.
//

#ifndef AJAZZ_HPP
#define AJAZZ_HPP

#include <iomanip>
#include <string>
#include <hidapi/hidapi.h>

#include "keyboarddefs.hpp"

class AK820Pro {
    struct ModePacket {
        uint8_t mode;
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint32_t padding;
        struct {
            bool rainbow;
            uint8_t brightness;
            uint8_t speed;
            uint8_t direction;
        } options;
        uint16_t padding1;
        uint16_t delimiter;
        uint8_t additional_data[48];
    };

public:
    enum LightingMode {
        LED_OFF = 0x00,
        STATIC = 0x01,
        SINGLE_ON = 0x02,
        SINGLE_OFF = 0x03,
        GLITTERING = 0x04,
        FALLING = 0x05,
        COLOURFUL = 0x06,
        BREATH = 0x07,
        SPECTRUM = 0x08,
        OUTWARD = 0x09,
        SCROLLING = 0x0a,
        ROLLING = 0x0b,
        ROTATING = 0x0c,
        EXPLODE = 0x0d,
        LAUNCH = 0x0e,
        RIPPLES = 0x0f,
        FLOWING = 0x10,
        PULSATING = 0x11,
        TILT = 0x12,
        SHUTTLE = 0x13,
    };

    enum DIRECTION {
        LEFT,
        UP,
        DOWN,
        RIGHT
    };

    enum LightSleepTime {
        NONE,
        ONE_MIN,
        FIVE_MIN,
        THIRTY_MIN
    };

    explicit AK820Pro(uint16_t vid, uint16_t pid);
    ~AK820Pro();

    void setColor(uint8_t r, uint8_t g, uint8_t b) const;
    void setMode(LightingMode mode, uint8_t r = 255, uint8_t g = 0 , uint8_t b = 0,
                bool rainbow = true, int brightness = MAX_BRIGHTNESS, int speed = MAX_SPEED, DIRECTION direction = LEFT);
    void setSleepTime(LightSleepTime sleep_time);
    void uploadGIF(const std::string& path) const;
private:
    uint16_t vid;
    uint16_t pid;
    hid_device* handle = nullptr;
};

#endif //AJAZZ_HPP