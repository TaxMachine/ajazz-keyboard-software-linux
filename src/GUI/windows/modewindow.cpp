//
// Created by taxmachine on 23/02/25.
// Modified by Vixcho on 13/10/26.
//

#include "../app.hpp"
#include <future>
#include <imgui.h>
#include <iostream>

#include "../../constants.hpp"
#include "../../utils/i18n.hpp"
#include "../../utils/logger.hpp"
#include "../../utils/rgb.hpp"

static int selectedMode = 1;
static int selectedTime = 0;
static int selectedDirection = 0;
static std::string lightingStatusMessage;

void AjazzGUI::changeMode() {
  if (!GetConfig())
    return;

  uint8_t r, g, b;
  const auto keyboard_color = RGB(GetConfig()->keyboard_rgb);
  keyboard_color.toRGB(&r, &g, &b);

  try {
    if (!GetKeyboard()->isConnected()) {
      GetKeyboard()->openHandle();
    }

    GetKeyboard()->applyLightingAsync(
        GetConfig()->mode, r, g, b, GetConfig()->rainbow,
        GetConfig()->brightness, GetConfig()->speed, GetConfig()->direction,
        GetConfig()->sleep_delay);

    GetConfig()->saveConfig();
  } catch (const std::exception &e) {
    Logger::getInstance().error(std::string("Error aplicando iluminación: ") +
                                e.what());
  }
}

void AjazzGUI::ModeWindow() {
  if (GetConfig()) {
    selectedMode = static_cast<int>(GetConfig()->mode);
    selectedTime = static_cast<int>(GetConfig()->sleep_delay);
    selectedDirection = static_cast<int>(GetConfig()->direction);
  }

  ImGui::TextColored(ImVec4(0.35f, 0.70f, 1.00f, 1.00f), "%s",
                     tr("mode_title"));
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Text("%s", tr("mode_lighting_mode"));
  int col = 0;
  for (const auto &[mode, name] : LIGHT_MODES) {
    if (ImGui::RadioButton(name, &selectedMode, mode)) {
      GetConfig()->mode = static_cast<AK820Pro::LightingMode>(mode);
      this->changeMode();
      lightingStatusMessage = tr("mode_applied_success");
    }
    col++;
    if (col % 4 != 0) {
      ImGui::SameLine(0, 16);
    }
  }

  ImGui::Dummy(ImVec2(0, 12));
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Text("%s", tr("mode_static_color"));
  if (ImGui::Checkbox(tr("mode_rainbow"), &GetConfig()->rainbow)) {
    this->changeMode();
    lightingStatusMessage = tr("mode_applied_success");
  }

  if (!GetConfig()->rainbow) {
    ImGui::SameLine(0, 20);
    ImGui::ColorEdit3("##Picker", GetConfig()->keyboard_rgb,
                      ImGuiColorEditFlags_NoAlpha);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
      this->changeMode();
      lightingStatusMessage = tr("mode_applied_success");
    }
  }

  ImGui::Dummy(ImVec2(0, 12));
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::SliderInt(tr("mode_brightness"), &GetConfig()->brightness, 0,
                   MAX_BRIGHTNESS);
  if (ImGui::IsItemDeactivatedAfterEdit()) {
    this->changeMode();
    lightingStatusMessage = tr("mode_applied_success");
  }

  ImGui::SliderInt(tr("mode_speed"), &GetConfig()->speed, 0, MAX_SPEED);
  if (ImGui::IsItemDeactivatedAfterEdit()) {
    this->changeMode();
    lightingStatusMessage = tr("mode_applied_success");
  }

  ImGui::Dummy(ImVec2(0, 12));
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Text("%s", tr("mode_sleep_time"));
  int sCol = 0;
  for (const auto &[mode, mode_name] : SLEEP_DELAY) {
    if (ImGui::RadioButton(mode_name, &selectedTime, mode)) {
      GetConfig()->sleep_delay = static_cast<AK820Pro::LightSleepTime>(mode);
      this->changeMode();
      lightingStatusMessage = tr("mode_applied_success");
    }
    sCol++;
    if (sCol < static_cast<int>(SLEEP_DELAY.size())) {
      ImGui::SameLine(0, 16);
    }
  }

  if (DIRECTIONS_PER_MODE.contains(GetConfig()->mode)) {
    ImGui::Dummy(ImVec2(0, 12));
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("%s", tr("mode_direction"));
    const auto &directions = DIRECTIONS_PER_MODE.at(GetConfig()->mode);
    for (size_t i = 0; i < directions.size(); i++) {
      auto dir = directions[i];
      if (ImGui::RadioButton(DIRECTIONS.at(dir), &selectedDirection, dir)) {
        GetConfig()->direction = static_cast<AK820Pro::Direction>(dir);
        this->changeMode();
        lightingStatusMessage = tr("mode_applied_success");
      }
      if (i + 1 < directions.size()) {
        ImGui::SameLine(0, 16);
      }
    }
  }

  ImGui::Dummy(ImVec2(0, 16));
  if (ImGui::Button(tr("btn_apply_lighting"), ImVec2(200, 40))) {
    this->changeMode();
    lightingStatusMessage = tr("mode_applied_success");
  }

  ImGui::SameLine(0, 12);
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.45f, 0.25f, 1.0f));
  if (ImGui::Button(tr("btn_turn_on_leds"), ImVec2(220, 40))) {
    GetConfig()->mode = AK820Pro::SPECTRUM;
    GetConfig()->brightness = MAX_BRIGHTNESS;
    GetConfig()->rainbow = true;
    GetConfig()->speed = 3;
    selectedMode = static_cast<int>(AK820Pro::SPECTRUM);
    this->changeMode();
    lightingStatusMessage = tr("mode_applied_success");
  }
  ImGui::PopStyleColor();

  ImGui::SameLine(0, 12);
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.40f, 0.20f, 0.20f, 1.0f));
  if (ImGui::Button(tr("btn_turn_off_leds"), ImVec2(160, 40))) {
    GetConfig()->mode = AK820Pro::LED_OFF;
    GetConfig()->brightness = 0;
    selectedMode = static_cast<int>(AK820Pro::LED_OFF);
    this->changeMode();
    lightingStatusMessage = tr("mode_applied_success");
  }
  ImGui::PopStyleColor();

  if (!lightingStatusMessage.empty()) {
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.40f, 1.0f), "%s",
                       lightingStatusMessage.c_str());
  }
}
