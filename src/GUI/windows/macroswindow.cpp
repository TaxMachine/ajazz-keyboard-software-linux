//
// Created by taxmachine on 26/02/25.
// Modified by Vixcho on 13/10/26.
//

#include "../app.hpp"
#include "../../utils/i18n.hpp"
#include <imgui.h>
#include <string>
#include <vector>

struct MacroAction {
  std::string key;
  bool isDown;
  int delayMs;
};

struct MacroItem {
  std::string name;
  std::string triggerKey;
  std::vector<MacroAction> actions;
};

static std::vector<MacroItem> macros = {
    {"Macro 1 (WASD Spam)",
     "F13",
     {{"W", true, 50}, {"W", false, 50}, {"A", true, 50}, {"A", false, 50}}},
    {"Macro 2 (GG WP)",
     "F14",
     {{"G", true, 40}, {"G", false, 40}, {"W", true, 40}, {"P", true, 40}}},
    {"Macro 3 (Burst Combo)",
     "F15",
     {{"Q", true, 30}, {"W", true, 30}, {"E", true, 30}, {"R", true, 30}}}};

static int selectedMacro = 0;
static char macroNameBuf[64] = "";
static char macroTriggerBuf[32] = "";
static char newKeyBuf[16] = "";
static int newDelayMs = 50;

void AjazzGUI::MacrosWindow() {
  ImGui::TextColored(ImVec4(0.35f, 0.70f, 1.00f, 1.00f), "%s",
                     tr("macros_title"));
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Columns(2, "MacroColumns", true);
  ImGui::SetColumnWidth(0, 260);

  ImGui::Text("%s", tr("macros_saved"));
  for (size_t i = 0; i < macros.size(); i++) {
    std::string label = macros[i].name + "##" + std::to_string(i);
    if (ImGui::Selectable(label.c_str(),
                          selectedMacro == static_cast<int>(i))) {
      selectedMacro = static_cast<int>(i);
      snprintf(macroNameBuf, sizeof(macroNameBuf), "%s",
               macros[i].name.c_str());
      snprintf(macroTriggerBuf, sizeof(macroTriggerBuf), "%s",
               macros[i].triggerKey.c_str());
    }
  }

  ImGui::Spacing();
  if (ImGui::Button(tr("macros_btn_new"), ImVec2(120, 32))) {
    macros.push_back({tr("macros_btn_new"), "F16", {}});
    selectedMacro = static_cast<int>(macros.size() - 1);
  }
  ImGui::SameLine();
  if (ImGui::Button(tr("macros_btn_delete"), ImVec2(100, 32)) && !macros.empty()) {
    macros.erase(macros.begin() + selectedMacro);
    if (selectedMacro >= static_cast<int>(macros.size())) {
      selectedMacro = static_cast<int>(macros.size()) - 1;
    }
  }

  ImGui::NextColumn();

  if (selectedMacro >= 0 && selectedMacro < static_cast<int>(macros.size())) {
    auto &current = macros[selectedMacro];

    ImGui::Text("%s", tr("macros_settings"));
    ImGui::InputText(tr("macros_name"), macroNameBuf, sizeof(macroNameBuf));
    ImGui::InputText(tr("macros_trigger"), macroTriggerBuf,
                     sizeof(macroTriggerBuf));

    if (ImGui::Button(tr("macros_save_meta"))) {
      current.name = macroNameBuf;
      current.triggerKey = macroTriggerBuf;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text(tr("macros_actions_seq"), current.actions.size());
    ImGui::BeginChild("##ActionsList", ImVec2(0, 200), true);
    for (size_t a = 0; a < current.actions.size(); a++) {
      const auto &act = current.actions[a];
      ImGui::Text("[%zu] %s '%s' (%s) - %d ms", a + 1,
                  tr("macros_key"),
                  act.key.c_str(), act.isDown ? tr("macros_press") : tr("macros_release"),
                  act.delayMs);
    }
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::InputText(tr("macros_key"), newKeyBuf, sizeof(newKeyBuf));
    ImGui::SameLine();
    ImGui::InputInt(tr("macros_delay"), &newDelayMs);

    if (ImGui::Button(tr("macros_btn_add_pulse"), ImVec2(150, 32))) {
      if (strlen(newKeyBuf) > 0) {
        current.actions.push_back({newKeyBuf, true, newDelayMs});
        current.actions.push_back({newKeyBuf, false, newDelayMs});
        newKeyBuf[0] = '\0';
      }
    }
    ImGui::SameLine();
    if (ImGui::Button(tr("macros_btn_clear"), ImVec2(130, 32))) {
      current.actions.clear();
    }
  }

  ImGui::Columns(1);
}
