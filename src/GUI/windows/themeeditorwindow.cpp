//
// Created by taxmachine on 26/02/25.
// Modified by Vixcho on 13/10/26.
//

#include "../../constants.hpp"
#include "../../utils/i18n.hpp"
#include "../app.hpp"
#include <imgui.h>

void AjazzGUI::ThemeEditorWindow() {
  ImGui::TextColored(ImVec4(0.35f, 0.70f, 1.00f, 1.00f), "%s",
                     tr("theme_title"));
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::BeginGroup();
  if (ImGui::SliderFloat(tr("theme_window_rounding"),
                         &GetConfig()->window_roundness, 0, 15)) {
    ImGui::GetStyle().WindowRounding = GetConfig()->window_roundness;
  }

  if (ImGui::SliderFloat(tr("theme_frame_rounding"),
                         &GetConfig()->frame_roundness, 0, 15)) {
    ImGui::GetStyle().FrameRounding = GetConfig()->frame_roundness;
  }

  if (ImGui::SliderFloat(tr("theme_padding_x"), &GetConfig()->frame_padding_x,
                         0, 15)) {
    ImGui::GetStyle().FramePadding.x = GetConfig()->frame_padding_x;
  }

  if (ImGui::SliderFloat(tr("theme_padding_y"), &GetConfig()->frame_padding_y,
                         0, 15)) {
    ImGui::GetStyle().FramePadding.y = GetConfig()->frame_padding_y;
  }
  ImGui::EndGroup();
}
