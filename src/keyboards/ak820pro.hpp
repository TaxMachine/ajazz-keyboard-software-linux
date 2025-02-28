//
// Created by taxmachine on 16/02/25.
//

#ifndef AJAZZ_HPP
#define AJAZZ_HPP

#include <future>
#include <iomanip>
#include <string>
#include <vector>
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
        LED_OFF,
        STATIC,
        SINGLE_ON,
        SINGLE_OFF,
        GLITTERING,
        FALLING,
        COLOURFUL,
        BREATH,
        SPECTRUM,
        OUTWARD,
        SCROLLING,
        ROLLING,
        ROTATING,
        EXPLODE,
        LAUNCH,
        RIPPLES,
        FLOWING,
        PULSATING,
        TILT,
        SHUTTLE,
    };

    enum Direction {
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

    void openHandle();
    void closeHandle();

    void setColor(uint8_t r, uint8_t g, uint8_t b) const;
    void setMode(LightingMode mode, uint8_t r = 255, uint8_t g = 0 , uint8_t b = 0,
                bool rainbow = true, int brightness = MAX_BRIGHTNESS, int speed = MAX_SPEED, Direction direction = LEFT) const;
    std::future<void> setModeAsync(const LightingMode mode, const uint8_t r, const uint8_t g, const uint8_t b,
                                   const bool rainbow, const int brightness, const int speed, const Direction direction) const;

    void setSleepTime(LightSleepTime sleep_time) const;
    std::future<void> setSleepTimeAsync(const LightSleepTime sleep_time) const;

    void uploadGIF(const std::string& path) const;
private:
    uint16_t vid;
    uint16_t pid;
    hid_device* handle = nullptr;

    void executeHIDCommand(const std::vector<uint8_t*>& commands) const;
};

#endif //AJAZZ_HPP