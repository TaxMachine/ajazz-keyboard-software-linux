//
// Created by taxmachine on 16/02/25.
// Modified by Vixcho on 13/10/26.
//

#include "config.hpp"

#include <fstream>
#include <imgui.h>
#include <ostream>

#include "i18n.hpp"
#include "rgb.hpp"
#include <filesystem>

std::string Config::getConfigFilePath() const {
  const char *xdg = secure_getenv("XDG_CONFIG_HOME");
  std::string base;
  if (xdg && strlen(xdg) > 0) {
    base = xdg;
  } else {
    const char *home = secure_getenv("HOME");
    base = (home && strlen(home) > 0) ? std::string(home) + "/.config" : ".";
  }
  std::string dir = base + "/ak820proLED";
  std::filesystem::create_directories(dir);
  return dir + "/config.json";
}

Config::Config() {
  std::string path = getConfigFilePath();
  std::ifstream file(path);
  if (!file.is_open()) {
    // Fallback check for old ~/.ajazz.json
    const char *home = secure_getenv("HOME");
    if (home) {
      std::string oldPath = std::string(home) + "/.ajazz.json";
      std::ifstream oldFile(oldPath);
      if (oldFile.is_open()) {
        try {
          this->config = nlohmann::json::parse(oldFile);
          this->loadConfig();
        } catch (...) {
        }
        oldFile.close();
      }
    }
    return;
  }
  try {
    this->config = nlohmann::json::parse(file);
    this->loadConfig();
  } catch (...) {
  }
  file.close();
}

Config::~Config() { this->saveConfig(); }

static RGB getRGBField(const nlohmann::json &field) {
  if (!field.is_object()) {
    return RGB(255, 255, 255, 255);
  }
  uint8_t r = field.value("r", (uint8_t)255);
  uint8_t g = field.value("g", (uint8_t)255);
  uint8_t b = field.value("b", (uint8_t)255);
  uint8_t a = field.value("a", (uint8_t)255);
  return RGB(r, g, b, a);
}

static nlohmann::json setRGBField(const float color[4]) {
  const auto rgb_color = RGB(color);
  uint8_t r, g, b;
  rgb_color.toRGB(&r, &g, &b);
  uint8_t a = static_cast<uint8_t>(color[3] * 255.0f);
  return {{"r", r}, {"g", g}, {"b", b}, {"a", a}};
}

void Config::loadConfig() {
  if (this->config.contains("theme") && this->config["theme"].is_object()) {
    const auto &key = this->config["theme"];
    if (key.contains("text_color"))
      getRGBField(key["text_color"]).toFloat(this->text_color);
    if (key.contains("separator_color"))
      getRGBField(key["separator_color"]).toFloat(this->separator_color);
    if (key.contains("separator_hovered_color"))
      getRGBField(key["separator_hovered_color"]).toFloat(this->separator_hovered_color);
    if (key.contains("button_color"))
      getRGBField(key["button_color"]).toFloat(this->button_color);
    if (key.contains("frame_bg_color"))
      getRGBField(key["frame_bg_color"]).toFloat(this->frame_bg_color);
    if (key.contains("header_color"))
      getRGBField(key["header_color"]).toFloat(this->header_color);
    if (key.contains("header_hovered_color"))
      getRGBField(key["header_hovered_color"]).toFloat(this->header_hovered_color);
    if (key.contains("window_bg_color"))
      getRGBField(key["window_bg_color"]).toFloat(this->window_bg_color);
    if (key.contains("child_bg_color"))
      getRGBField(key["child_bg_color"]).toFloat(this->child_bg_color);
    if (key.contains("title_bg_color"))
      getRGBField(key["title_bg_color"]).toFloat(this->title_bg_color);
    if (key.contains("title_bg_active"))
      getRGBField(key["title_bg_active"]).toFloat(this->title_bg_active);
    if (key.contains("title_bg_collapsed_color"))
      getRGBField(key["title_bg_collapsed_color"]).toFloat(this->title_bg_collapsed_color);

    this->frame_padding_x = key.value("frame_padding_x", 8.0f);
    this->frame_padding_y = key.value("frame_padding_y", 4.0f);
    this->window_roundness = key.value("window_roundness", 6.0f);
    this->frame_roundness = key.value("frame_roundness", 4.0f);
  } else {
    ImVec4 *style = ImGui::GetStyle().Colors;
    constexpr int ImGuiColSize = sizeof(float) * 4;

    memcpy(this->text_color, &style[ImGuiCol_Text], ImGuiColSize);
    memcpy(this->separator_color, &style[ImGuiCol_Separator], ImGuiColSize);
    memcpy(this->separator_hovered_color, &style[ImGuiCol_SeparatorHovered],
           ImGuiColSize);
    memcpy(this->button_color, &style[ImGuiCol_Button], ImGuiColSize);
    memcpy(this->frame_bg_color, &style[ImGuiCol_FrameBg], ImGuiColSize);
    memcpy(this->header_color, &style[ImGuiCol_Header], ImGuiColSize);
    memcpy(this->header_hovered_color, &style[ImGuiCol_HeaderHovered],
           ImGuiColSize);
    memcpy(this->window_bg_color, &style[ImGuiCol_WindowBg], ImGuiColSize);
    memcpy(this->child_bg_color, &style[ImGuiCol_ChildBg], ImGuiColSize);
    memcpy(this->title_bg_color, &style[ImGuiCol_TitleBg], ImGuiColSize);
    memcpy(this->title_bg_active, &style[ImGuiCol_TitleBgActive], ImGuiColSize);
    memcpy(this->title_bg_collapsed_color, &style[ImGuiCol_TitleBgCollapsed],
           ImGuiColSize);
    this->frame_padding_x = 8.0f;
    this->frame_padding_y = 4.0f;
    this->window_roundness = 6.0f;
    this->frame_roundness = 4.0f;
  }

  if (this->config.contains("keyboard") && this->config["keyboard"].is_object()) {
    const auto &key = this->config["keyboard"];

    this->mode = static_cast<AK820Pro::LightingMode>(key.value("mode", 1));
    this->sleep_delay = static_cast<AK820Pro::LightSleepTime>(key.value("sleep_delay", 0));
    this->direction = static_cast<AK820Pro::Direction>(key.value("direction", 2));

    if (key.contains("keyboard_rgb")) {
      getRGBField(key["keyboard_rgb"]).toFloat(this->keyboard_rgb);
    } else {
      this->keyboard_rgb[0] = 1.0f;
      this->keyboard_rgb[1] = 0.0f;
      this->keyboard_rgb[2] = 0.0f;
    }
    this->brightness = key.value("brightness", MAX_BRIGHTNESS);
    this->speed = key.value("speed", MAX_SPEED);
    this->rainbow = key.value("rainbow", true);
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

  if (this->config.contains("app") && this->config["app"].is_object()) {
    const auto &appKey = this->config["app"];
    this->language = appKey.value("language", "en");
    this->auto_connect = appKey.value("auto_connect", true);
  }
  I18n::getInstance().setLanguageCode(this->language);
}

void Config::saveConfig() const {
  float rgb4[4] = {this->keyboard_rgb[0], this->keyboard_rgb[1],
                   this->keyboard_rgb[2], 1.0f};
  const nlohmann::json savefile = {
      {"theme",
       {{"text_color", setRGBField(this->text_color)},
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
        {"title_bg_collapsed_color",
         setRGBField(this->title_bg_collapsed_color)},
        {"frame_padding_x", this->frame_padding_x},
        {"frame_padding_y", this->frame_padding_y},
        {"window_roundness", this->window_roundness},
        {"frame_roundness", this->frame_roundness}}},
      {"keyboard",
       {{"mode", static_cast<int>(this->mode)},
        {"sleep_delay", static_cast<int>(this->sleep_delay)},
        {"direction", static_cast<int>(this->direction)},
        {"keyboard_rgb", setRGBField(rgb4)},
        {"brightness", this->brightness},
        {"speed", this->speed},
        {"rainbow", this->rainbow}}},
      {"app",
       {{"language", this->language}, {"auto_connect", this->auto_connect}}}};

  std::string path = getConfigFilePath();
  std::ofstream file(path);
  if (!file.is_open()) {
    return;
  }

  file << savefile.dump(4);
  file.close();
}
