//
// Created by taxmachine on 26/02/25.
//

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../keyboards/ak820pro.hpp"
#include "json.hpp"

class Config {
public:
    explicit Config();
    ~Config();

    void saveConfig() const;
    void loadConfig();

    /* imgui theme config */
    float text_color[4];
    float separator_color[4];
    float separator_hovered_color[4];
    float button_color[4];
    float frame_bg_color[4];
    float header_color[4];
    float header_hovered_color[4];
    float window_bg_color[4];
    float child_bg_color[4];
    float title_bg_color[4];
    float title_bg_active[4];
    float title_bg_collapsed_color[4];
    float frame_padding_x;
    float frame_padding_y;
    float window_roundness;
    float frame_roundness;

    /* keyboard mode config */
    AK820Pro::LightingMode mode{};
    AK820Pro::LightSleepTime sleep_delay{};
    AK820Pro::Direction direction{};
    float keyboard_rgb[3] = {1.0f, 0.0f, 0.0f};
    int brightness = MAX_BRIGHTNESS;
    int speed = MAX_SPEED;
    bool rainbow = true;

private:
    nlohmann::json config;
};

#endif //CONFIG_HPP
