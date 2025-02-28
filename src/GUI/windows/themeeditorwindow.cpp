//
// Created by taxmachine on 26/02/25.
//
#include "../app.hpp"

#include <imgui.h>

#include "../../constants.hpp"

void AjazzGUI::ThemeEditorWindow() {
    ImGui::BeginGroup();

    ImGui::BeginGroup();
    if (ImGui::SliderFloat("Window Rounding", &GetConfig()->window_roundness, 0, 15)) {
        ImGui::GetStyle().WindowRounding = GetConfig()->window_roundness;
    }

    if (ImGui::SliderFloat("Frame Rounding", &GetConfig()->frame_roundness, 0, 15)) {
        ImGui::GetStyle().FrameRounding = GetConfig()->frame_roundness;
    }

    if (ImGui::SliderFloat("Frame Padding (X)", &GetConfig()->frame_padding_x, 0, 15)) {
        ImGui::GetStyle().FramePadding.x = GetConfig()->frame_padding_x;
    }

    if (ImGui::SliderFloat("Frame Padding (Y)", &GetConfig()->frame_padding_y, 0, 15)) {
        ImGui::GetStyle().FramePadding.y = GetConfig()->frame_padding_y;
    }

    ImGui::EndGroup();

    ImGui::EndGroup();
}
