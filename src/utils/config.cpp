//
// Created by taxmachine on 26/02/25.
//

#include "config.hpp"

#include <fstream>
#include <ostream>
#include <imgui.h>

#include "rgb.hpp"

Config::Config() {
    std::string home = secure_getenv("HOME");
    std::string path = home + "/.ajazz.json";
    std::ifstream file(path);
    if (!file.is_open()) {
        return;
    }
    if (file.tellg() > 0) {
        this->config = nlohmann::json::parse(file);
    }
    this->loadConfig();
    file.close();
}

Config::~Config() {
    this->saveConfig();
}

static RGB getRGBField(const nlohmann::json& field) {
    const uint8_t r = field["r"].get<uint8_t>();
    const uint8_t g = field["g"].get<uint8_t>();
    const uint8_t b = field["b"].get<uint8_t>();
    const uint8_t a = field["a"].get<uint8_t>();

    return RGB(r, g, b, a);
}

static nlohmann::json setRGBField(const float color[4]) {
    const auto rgb_color = RGB(color);
    uint8_t r, g, b;
    rgb_color.toRGB(&r, &g, &b);
    return {
        {"r", r},
        {"g", g},
        {"b", b}
    };
}

void Config::loadConfig() {
    if (this->config.contains("theme")) {
        nlohmann::json key = this->config["theme"];
        const RGB text_color_rgb = getRGBField(key["text_color"]);
        text_color_rgb.toFloat(this->text_color);

        const RGB separator_color_rgb = getRGBField(key["separator_color"]);
        separator_color_rgb.toFloat(this->separator_color);

        const RGB separator_hovered_color_rgb = getRGBField(key["separator_hovered_color"]);
        separator_hovered_color_rgb.toFloat(this->separator_hovered_color);

        const RGB button_color_rgb = getRGBField(key["button_color"]);
        button_color_rgb.toFloat(this->button_color);

        const RGB frame_bg_color_rgb = getRGBField(key["frame_bg_color"]);
        frame_bg_color_rgb.toFloat(this->frame_bg_color);

        const RGB header_color_rgb = getRGBField(key["header_color"]);
        header_color_rgb.toFloat(this->header_color);

        const RGB header_hovered_color_rgb = getRGBField(key["header_hovered_color"]);
        header_hovered_color_rgb.toFloat(this->header_hovered_color);

        const RGB window_bg_color_rgb = getRGBField(key["window_bg_color"]);
        window_bg_color_rgb.toFloat(this->window_bg_color);

        const RGB child_bg_color_rgb = getRGBField(key["child_bg_color"]);
        child_bg_color_rgb.toFloat(this->child_bg_color);

        const RGB title_bg_color_rgb = getRGBField(key["title_bg_color"]);
        title_bg_color_rgb.toFloat(this->title_bg_color);

        const RGB title_bg_active_rgb = getRGBField(key["title_bg_active"]);
        title_bg_active_rgb.toFloat(this->title_bg_active);

        const RGB title_bg_collapsed_color_rgb = getRGBField(key["title_bg_collapsed_color"]);
        title_bg_collapsed_color_rgb.toFloat(this->title_bg_collapsed_color);

        this->frame_padding_x = key["frame_padding_x"].get<float>();
        this->frame_padding_y = key["frame_padding_y"].get<float>();
        this->window_roundness = key["window_roundness"].get<float>();
        this->frame_roundness = key["frame_roundness"].get<float>();
    } else {
        ImVec4 *style = ImGui::GetStyle().Colors;
        constexpr int ImGuiColSize = sizeof(float) * 4;

        memcpy(this->text_color, &style[ImGuiCol_Text], ImGuiColSize);
        memcpy(this->separator_color, &style[ImGuiCol_Separator], ImGuiColSize);
        memcpy(this->separator_hovered_color, &style[ImGuiCol_SeparatorHovered], ImGuiColSize);
        memcpy(this->button_color, &style[ImGuiCol_Button], ImGuiColSize);
        memcpy(this->frame_bg_color, &style[ImGuiCol_FrameBg], ImGuiColSize);
        memcpy(this->header_color, &style[ImGuiCol_Header], ImGuiColSize);
        memcpy(this->header_hovered_color, &style[ImGuiCol_HeaderHovered], ImGuiColSize);
        memcpy(this->window_bg_color, &style[ImGuiCol_WindowBg], ImGuiColSize);
        memcpy(this->child_bg_color, &style[ImGuiCol_ChildBg], ImGuiColSize);
        memcpy(this->title_bg_color, &style[ImGuiCol_TitleBg], ImGuiColSize);
        memcpy(this->title_bg_active, &style[ImGuiCol_TitleBgActive], ImGuiColSize);
        memcpy(this->title_bg_collapsed_color, &style[ImGuiCol_TitleBgCollapsed], ImGuiColSize);
    }

    if (this->config.contains("keyboard")) {
        nlohmann::json key = this->config["keyboard"];

        this->mode = static_cast<AK820Pro::LightingMode>(key["mode"].get<int>());
        this->sleep_delay = static_cast<AK820Pro::LightSleepTime>(key["sleep_delay"].get<int>());
        this->direction = static_cast<AK820Pro::Direction>(key["direction"].get<int>());

        const RGB keyboard_rgb_rgb = getRGBField(key["keyboard_rgb"]);
        keyboard_rgb_rgb.toFloat(this->keyboard_rgb);
        this->brightness = key["brightness"].get<int>();
        this->speed = key["speed"].get<int>();
        this->rainbow = key["rainbow"].get<int>();
    } else {
        this->mode = AK820Pro::STATIC;
        this->sleep_delay = AK820Pro::NONE;
        this->direction = AK820Pro::UP;
        float default_color[3] = {1.0f, 0.0f, 0.0f};
        memcpy(this->keyboard_rgb, &default_color, sizeof(float) * 3);
        this->brightness = MAX_BRIGHTNESS;
        this->speed = MAX_SPEED;
        this->rainbow = true;
    }
}

void Config::saveConfig() const {
    const nlohmann::json savefile = {
        {"theme", {
            {"text_color", setRGBField(this->text_color)},
            {"separator_color", setRGBField(this->separator_color)},
            {"separator_hovered_color", setRGBField(this->separator_hovered_color)},
            {"button_color", setRGBField(this->button_color)},
            {"frame_bg_color", setRGBField(this->frame_bg_color)},
            {"header_color", setRGBField(this->header_color)},
            {"header_hovered_color", setRGBField(this->header_hovered_color)},
            {"window_bg_color", setRGBField(this->window_bg_color)},
            {"child_bg_color", setRGBField(this->child_bg_color)},
            {"title_bg_color", setRGBField(this->title_bg_color)},
            {"title_bg_active", setRGBField(this->title_bg_active)},
            {"title_bg_collapsed_color", setRGBField(this->title_bg_collapsed_color)},
            {"frame_padding_x", this->frame_padding_x},
            {"frame_padding_y", this->frame_padding_y},
            {"frame_roundness", this->frame_roundness}
        }},
        {"keyboard", {
            {"mode", this->mode},
            {"sleep_delay", this->sleep_delay},
            {"direction", this->direction},
            {"brightness", this->brightness},
            {"speed", this->speed},
            {"rainbow", this->rainbow}
        }}
    };


    std::string home = secure_getenv("HOME");
    std::string path = home + "/.ajazz.json";
    std::ofstream file(path);
    if (!file.is_open()) {
        return;
    }

    file << nlohmann::to_string(savefile);

    file.close();
}
