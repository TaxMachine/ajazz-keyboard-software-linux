//
// Created by Vixcho on 13/10/26.
//

#include "../../utils/logger.hpp"
#include "../../utils/i18n.hpp"
#include "../app.hpp"
#include <imgui.h>
#include <string>
#include <vector>

static bool autoScroll = true;
static bool copiedLogs = false;

void AjazzGUI::LogsWindow() {
  ImGui::TextColored(ImVec4(0.35f, 0.70f, 1.00f, 1.00f), "%s",
                     tr("logs_title"));
  ImGui::Separator();
  ImGui::Spacing();

  // Toolbar buttons
  if (ImGui::Button(tr("btn_copy_all_logs"), ImVec2(180, 34))) {
    std::string allLogs = Logger::getInstance().getAllText();
    ImGui::SetClipboardText(allLogs.c_str());
    copiedLogs = true;
  }

  ImGui::SameLine(0, 10);
  if (ImGui::Button(tr("btn_clear_logs"), ImVec2(120, 34))) {
    Logger::getInstance().clear();
    copiedLogs = false;
  }

  ImGui::SameLine(0, 15);
  ImGui::Checkbox(tr("logs_auto_scroll"), &autoScroll);

  if (copiedLogs) {
    ImGui::SameLine(0, 15);
    ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.4f, 1.0f), "%s",
                       tr("logs_copied_msg"));
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // Log display area
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.07f, 0.08f, 0.10f, 1.00f));
  ImGui::BeginChild("##LogScrollRegion", ImVec2(0, 0), true,
                    ImGuiWindowFlags_HorizontalScrollbar);

  auto entries = Logger::getInstance().getEntries();
  if (entries.empty()) {
    ImGui::TextDisabled("%s", tr("logs_empty"));
  } else {
    for (const auto &entry : entries) {
      ImVec4 color = ImVec4(0.75f, 0.80f, 0.88f, 1.00f);
      const char *levelStr = "[INFO]";

      switch (entry.level) {
      case LogLevel::SUCCESS:
        color = ImVec4(0.30f, 0.90f, 0.45f, 1.00f);
        levelStr = "[OK]";
        break;
      case LogLevel::WARNING:
        color = ImVec4(0.95f, 0.75f, 0.25f, 1.00f);
        levelStr = "[WARN]";
        break;
      case LogLevel::ERR:
        color = ImVec4(0.95f, 0.35f, 0.35f, 1.00f);
        levelStr = "[ERROR]";
        break;
      case LogLevel::INFO:
      default:
        color = ImVec4(0.55f, 0.75f, 1.00f, 1.00f);
        levelStr = "[INFO]";
        break;
      }

      ImGui::TextDisabled("[%s]", entry.timestamp.c_str());
      ImGui::SameLine(0, 6);
      ImGui::TextColored(color, "%s", levelStr);
      ImGui::SameLine(0, 6);
      ImGui::TextUnformatted(entry.message.c_str());
    }

    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
      ImGui::SetScrollHereY(1.0f);
    }
  }

  ImGui::EndChild();
  ImGui::PopStyleColor();
}
