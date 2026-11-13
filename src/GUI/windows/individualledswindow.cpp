//
// Created by Vixcho on 13/10/26.
//

#include "../../constants.hpp"
#include "../../utils/i18n.hpp"
#include "../app.hpp"
#include <imgui.h>
#include <iostream>
#include <string>
#include <vector>

struct KeyDef {
  const char *label;
  uint8_t index;
  float width;
};

static float brushColor[3] = {1.0f, 0.0f, 0.0f};
static float keyColorsRGB[144][3] = {{0}};
static bool initializedKeys = false;
static std::string statusMessage = "";

void AjazzGUI::IndividualLEDsWindow() {
  if (!initializedKeys) {
    for (int i = 0; i < 144; i++) {
      keyColorsRGB[i][0] = 0.2f;
      keyColorsRGB[i][1] = 0.5f;
      keyColorsRGB[i][2] = 0.8f;
    }
    initializedKeys = true;
  }

  ImGui::TextColored(ImVec4(0.35f, 0.70f, 1.00f, 1.00f), "%s",
                     tr("leds_title"));
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Text("%s", tr("leds_brush_color"));
  ImGui::SameLine();
  ImGui::ColorEdit3("##BrushColor", brushColor, ImGuiColorEditFlags_NoAlpha);

  ImGui::SameLine(0, 20);
  if (ImGui::Button(tr("leds_btn_wasd"))) {
    uint8_t wasd[] = {17, 30, 31, 32}; // W, A, S, D positions
    for (uint8_t idx : wasd) {
      keyColorsRGB[idx][0] = brushColor[0];
      keyColorsRGB[idx][1] = brushColor[1];
      keyColorsRGB[idx][2] = brushColor[2];
    }
  }
  ImGui::SameLine(0, 6);
  if (ImGui::Button(tr("leds_btn_arrows"))) {
    uint8_t arrows[] = {57, 72, 73, 74};
    for (uint8_t idx : arrows) {
      keyColorsRGB[idx][0] = brushColor[0];
      keyColorsRGB[idx][1] = brushColor[1];
      keyColorsRGB[idx][2] = brushColor[2];
    }
  }
  ImGui::SameLine(0, 6);
  if (ImGui::Button(tr("leds_btn_all"))) {
    for (int i = 0; i < 144; i++) {
      keyColorsRGB[i][0] = brushColor[0];
      keyColorsRGB[i][1] = brushColor[1];
      keyColorsRGB[i][2] = brushColor[2];
    }
  }
  ImGui::SameLine(0, 6);
  if (ImGui::Button(tr("leds_btn_off"))) {
    for (int i = 0; i < 144; i++) {
      keyColorsRGB[i][0] = 0.0f;
      keyColorsRGB[i][1] = 0.0f;
      keyColorsRGB[i][2] = 0.0f;
    }
  }

  ImGui::Dummy(ImVec2(0, 10));

  // Keyboard Visualizer Rows
  const std::vector<std::vector<KeyDef>> layout = {{{"Esc", 0, 42},
                                                    {"F1", 1, 38},
                                                    {"F2", 2, 38},
                                                    {"F3", 3, 38},
                                                    {"F4", 4, 38},
                                                    {"F5", 5, 38},
                                                    {"F6", 6, 38},
                                                    {"F7", 7, 38},
                                                    {"F8", 8, 38},
                                                    {"F9", 9, 38},
                                                    {"F10", 10, 38},
                                                    {"F11", 11, 38},
                                                    {"F12", 12, 38},
                                                    {"Del", 13, 42},
                                                    {"Knob", 14, 48}},
                                                   {{"`~", 15, 38},
                                                    {"1", 16, 38},
                                                    {"2", 17, 38},
                                                    {"3", 18, 38},
                                                    {"4", 19, 38},
                                                    {"5", 20, 38},
                                                    {"6", 21, 38},
                                                    {"7", 22, 38},
                                                    {"8", 23, 38},
                                                    {"9", 24, 38},
                                                    {"0", 25, 38},
                                                    {"-_", 26, 38},
                                                    {"=+", 27, 38},
                                                    {"Backspace", 28, 76},
                                                    {"PgUp", 29, 44}},
                                                   {{"Tab", 30, 56},
                                                    {"Q", 31, 38},
                                                    {"W", 32, 38},
                                                    {"E", 33, 38},
                                                    {"R", 34, 38},
                                                    {"T", 35, 38},
                                                    {"Y", 36, 38},
                                                    {"U", 37, 38},
                                                    {"I", 38, 38},
                                                    {"O", 39, 38},
                                                    {"P", 40, 38},
                                                    {"[{", 41, 38},
                                                    {"]}", 42, 38},
                                                    {"\\|", 43, 56},
                                                    {"PgDn", 44, 44}},
                                                   {{"Caps", 45, 68},
                                                    {"A", 46, 38},
                                                    {"S", 47, 38},
                                                    {"D", 48, 38},
                                                    {"F", 49, 38},
                                                    {"G", 50, 38},
                                                    {"H", 51, 38},
                                                    {"J", 52, 38},
                                                    {"K", 53, 38},
                                                    {"L", 54, 38},
                                                    {";:", 55, 38},
                                                    {"'\"", 56, 38},
                                                    {"Enter", 57, 86},
                                                    {"End", 58, 44}},
                                                   {{"LShift", 59, 86},
                                                    {"Z", 60, 38},
                                                    {"X", 61, 38},
                                                    {"C", 62, 38},
                                                    {"V", 63, 38},
                                                    {"B", 64, 38},
                                                    {"N", 65, 38},
                                                    {"M", 66, 38},
                                                    {",<", 67, 38},
                                                    {".>", 68, 38},
                                                    {"/?", 69, 38},
                                                    {"RShift", 70, 72},
                                                    {"Up", 71, 38}},
                                                   {{"LCtrl", 72, 48},
                                                    {"Win", 73, 44},
                                                    {"LAlt", 74, 44},
                                                    {"Space", 75, 230},
                                                    {"RAlt", 76, 44},
                                                    {"Fn", 77, 44},
                                                    {"Left", 78, 38},
                                                    {"Down", 79, 38},
                                                    {"Right", 80, 38}}};

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

  for (const auto &row : layout) {
    for (size_t k = 0; k < row.size(); k++) {
      const auto &key = row[k];
      uint8_t idx = key.index;

      ImVec4 kColor(keyColorsRGB[idx][0], keyColorsRGB[idx][1],
                    keyColorsRGB[idx][2], 1.0f);
      ImGui::PushStyleColor(ImGuiCol_Button, kColor);
      ImGui::PushStyleColor(
          ImGuiCol_ButtonHovered,
          ImVec4(kColor.x + 0.15f, kColor.y + 0.15f, kColor.z + 0.15f, 1.0f));
      ImGui::PushStyleColor(
          ImGuiCol_ButtonActive,
          ImVec4(kColor.x - 0.1f, kColor.y - 0.1f, kColor.z - 0.1f, 1.0f));

      std::string btnId = std::string(key.label) + "##" + std::to_string(idx);
      if (ImGui::Button(btnId.c_str(), ImVec2(key.width, 36))) {
        keyColorsRGB[idx][0] = brushColor[0];
        keyColorsRGB[idx][1] = brushColor[1];
        keyColorsRGB[idx][2] = brushColor[2];
      }

      ImGui::PopStyleColor(3);

      if (k + 1 < row.size()) {
        ImGui::SameLine();
      }
    }
  }

  ImGui::PopStyleVar(2);

  ImGui::Dummy(ImVec2(0, 16));
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button(tr("btn_apply_individual_leds"), ImVec2(220, 42))) {
    try {
      std::vector<AK820Pro::KeyColor> keyColors;
      for (int i = 0; i < 144; i++) {
        uint8_t r = static_cast<uint8_t>(keyColorsRGB[i][0] * 255.0f);
        uint8_t g = static_cast<uint8_t>(keyColorsRGB[i][1] * 255.0f);
        uint8_t b = static_cast<uint8_t>(keyColorsRGB[i][2] * 255.0f);
        keyColors.push_back({static_cast<uint8_t>(i), r, g, b});
      }

      GetKeyboard()->setPerKeyRGBAsync(keyColors);
      statusMessage = tr("leds_success");
    } catch (const std::exception &e) {
      statusMessage = std::string(tr("leds_error")) + e.what();
    }
  }

  if (!statusMessage.empty()) {
    ImGui::SameLine(0, 15);
    ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.5f, 1.0f), "%s",
                       statusMessage.c_str());
  }
}
