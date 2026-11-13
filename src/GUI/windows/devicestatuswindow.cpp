//
// devicestatuswindow.cpp
// Connection Diagnostics & Linux udev Permissions
//

#include "../app.hpp"
#include <imgui.h>
#include <string>
#include "../../constants.hpp"
#include "../../utils/logger.hpp"
#include "../../utils/i18n.hpp"

static bool commandCopied = false;

void AjazzGUI::DeviceStatusWindow() {
    ImGui::TextColored(ImVec4(0.35f, 0.70f, 1.00f, 1.00f), "%s", tr("dev_title"));
    ImGui::Separator();
    ImGui::Spacing();

    bool connected = GetKeyboard() && GetKeyboard()->isConnected();
    bool wireless = GetKeyboard() && GetKeyboard()->isWireless();

    if (connected) {
        ImGui::TextColored(ImVec4(0.25f, 0.85f, 0.45f, 1.00f), "%s", tr("dev_connected"));
        ImGui::BulletText("%s", (std::string(tr("dev_conn_type")) + (wireless ? tr("dev_type_wireless") : tr("dev_type_wired"))).c_str());
        ImGui::BulletText("%s", tr("dev_vendor_id"));

        char pidStr[32];
        snprintf(pidStr, sizeof(pidStr), "0x%04x", GetKeyboard()->getPid());
        ImGui::BulletText("Product ID (PID): %s (%s)",
                          pidStr,
                          wireless ? tr("dev_type_wireless") : "AK820 Pro Wired");

        const char* tftStatus = GetKeyboard()->isDisplayAvailable()
            ? tr("dev_tft_ready")
            : (wireless ? tr("dev_tft_wireless_warn") : tr("dev_tft_not_found"));
        ImGui::BulletText("%s: %s", tr("tab_tft_screen"), tftStatus);
    } else {
        ImGui::TextColored(ImVec4(0.90f, 0.35f, 0.35f, 1.00f), "%s", tr("dev_disconnected"));
        if (GetKeyboard()) {
            std::string err = GetKeyboard()->getLastError();
            if (!err.empty()) {
                ImGui::TextColored(ImVec4(0.95f, 0.65f, 0.25f, 1.00f), "Detail: %s", err.c_str());
            }
        }
    }

    ImGui::Spacing();
    if (ImGui::Button(tr("btn_retry_conn"), ImVec2(240, 38))) {
        if (GetKeyboard()) {
            Logger::getInstance().info("Scanning for keyboard connection...");
            bool res = GetKeyboard()->openHandle();
            if (res) {
                Logger::getInstance().success("Keyboard connected: " + GetKeyboard()->getConnectionStatus());
            } else {
                Logger::getInstance().error("Connection failed: " + GetKeyboard()->getLastError());
            }
        }
    }

    ImGui::Dummy(ImVec2(0, 16));
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.90f, 0.80f, 0.30f, 1.00f), "%s", tr("dev_udev_title"));
    ImGui::TextWrapped("%s", tr("dev_udev_desc"));
    ImGui::Spacing();

    // Universal udev rule command (zero personal paths, works on any Linux distribution)
    std::string udevCommand =
        "echo 'SUBSYSTEM==\"hidraw\", ATTRS{idVendor}==\"0c45\", MODE=\"0666\"' | "
        "sudo tee /etc/udev/rules.d/99-ajazz-keyboard.rules && "
        "sudo udevadm control --reload-rules && sudo udevadm trigger";

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.07f, 0.08f, 1.00f));
    ImGui::BeginChild("##CodeBlock", ImVec2(0, 56), true);
    ImGui::TextUnformatted(udevCommand.c_str());
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();
    if (ImGui::Button(tr("btn_copy_udev"), ImVec2(260, 36))) {
        ImGui::SetClipboardText(udevCommand.c_str());
        commandCopied = true;
        Logger::getInstance().info("Universal udev command copied to clipboard.");
    }

    if (commandCopied) {
        ImGui::SameLine(0, 15);
        ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.4f, 1.0f), "%s", tr("dev_copied_success"));
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.70f, 0.75f, 0.80f, 1.00f), "%s", tr("dev_udev_modes"));
    ImGui::TextDisabled("%s", tr("dev_reconnect_note"));
}
