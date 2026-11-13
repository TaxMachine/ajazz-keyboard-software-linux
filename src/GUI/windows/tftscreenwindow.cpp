//
// tftscreenwindow.cpp
// TFT Screen Display Control, GIF/Image Upload, Live Preview, and Image Gallery
// Created by Vixcho on 13/10/26.
//

#include "../app.hpp"
#include <GL/gl.h>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <imgui.h>
#include <iostream>
#include <string>
#include <vector>

#include "../../constants.hpp"
#include "../../utils/i18n.hpp"
#include "../../utils/logger.hpp"

static char imagePathBuf[512] = "";
static std::string tftStatusMessage = "";
static int selectedSlot = 0;
static GLuint previewTexture = 0;
static std::string loadedPreviewPath = "";

struct GalleryItem {
  std::string path;
  std::string filename;
  GLuint texture = 0;
};
static std::vector<GalleryItem> galleryItems;
static bool galleryScanned = false;

static GLuint createTextureFromPath(const std::string &path) {
  if (!std::filesystem::exists(path))
    return 0;
  GError *error = nullptr;
  GdkPixbuf *pixbuf =
      gdk_pixbuf_new_from_file_at_scale(path.c_str(), 128, 128, FALSE, &error);
  if (!pixbuf) {
    if (error)
      g_error_free(error);
    return 0;
  }

  int w = gdk_pixbuf_get_width(pixbuf);
  int h = gdk_pixbuf_get_height(pixbuf);
  int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
  int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
  const guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, rowstride / n_channels);
  GLenum format = (n_channels == 4) ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE,
               pixels);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  g_object_unref(pixbuf);
  return tex;
}

static void refreshGallery() {
  for (auto &item : galleryItems) {
    if (item.texture != 0) {
      glDeleteTextures(1, &item.texture);
    }
  }
  galleryItems.clear();

  const char *home = secure_getenv("HOME");
  if (!home)
    return;
  std::string galDir = std::string(home) + "/.config/ak820proLED/gallery";
  if (!std::filesystem::exists(galDir))
    return;

  for (const auto &entry : std::filesystem::directory_iterator(galDir)) {
    if (entry.is_regular_file()) {
      std::string ext = entry.path().extension().string();
      if (ext == ".png" || ext == ".gif" || ext == ".jpg" || ext == ".jpeg") {
        GalleryItem item;
        item.path = entry.path().string();
        item.filename = entry.path().filename().string();
        item.texture = createTextureFromPath(item.path);
        galleryItems.push_back(item);
      }
    }
  }
  galleryScanned = true;
}

void AjazzGUI::TFTScreenWindow() {
  ImGui::TextColored(ImVec4(0.35f, 0.70f, 1.00f, 1.00f), "%s", tr("tft_title"));
  ImGui::Separator();
  ImGui::Spacing();

  // Section 1: RTC Clock Sync
  ImGui::Text("%s", tr("tft_clock_section"));
  ImGui::TextDisabled("%s", tr("tft_clock_desc"));

  auto now = std::chrono::system_clock::now();
  std::time_t tt = std::chrono::system_clock::to_time_t(now);
  std::string timeStr = std::ctime(&tt);
  if (!timeStr.empty() && timeStr.back() == '\n')
    timeStr.pop_back();

  ImGui::Text(tr("tft_sys_time"), timeStr.c_str());

  if (ImGui::Button(tr("btn_sync_clock"), ImVec2(260, 36))) {
    try {
      if (!GetKeyboard()->isConnected()) {
        GetKeyboard()->openHandle();
      }
      GetKeyboard()->syncClockAsync(1);
      tftStatusMessage = tr("tft_clock_success");
    } catch (const std::exception &e) {
      tftStatusMessage = std::string("Error: ") + e.what();
    }
  }

  ImGui::Dummy(ImVec2(0, 14));
  ImGui::Separator();
  ImGui::Spacing();

  // Section 2: LCD Image & GIF Upload
  ImGui::Text("%s", tr("tft_upload_section"));
  ImGui::TextDisabled("%s", tr("tft_upload_desc"));

  bool isWired = GetKeyboard() && GetKeyboard()->isConnected() &&
                 !GetKeyboard()->isWireless();
  bool dispReady = GetKeyboard() && GetKeyboard()->isDisplayAvailable();

  if (!isWired) {
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.95f, 0.55f, 0.20f, 1.0f));
    ImGui::BeginChild("##TFTNotice", ImVec2(0, 52), true);
    ImGui::TextColored(ImVec4(0.95f, 0.70f, 0.25f, 1.0f), "%s",
                       tr("tft_hardware_warn_title"));
    ImGui::TextWrapped("%s", tr("tft_hardware_warn_msg"));
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::Spacing();
  } else if (dispReady) {
    ImGui::TextColored(ImVec4(0.35f, 0.85f, 0.45f, 1.0f), "%s",
                       tr("tft_channel_ready"));
    ImGui::Spacing();
  }

  ImGui::SetNextItemWidth(450);
  if (ImGui::InputText(tr("tft_file_path"), imagePathBuf,
                       sizeof(imagePathBuf))) {
    if (loadedPreviewPath != imagePathBuf &&
        std::filesystem::exists(imagePathBuf)) {
      if (previewTexture != 0)
        glDeleteTextures(1, &previewTexture);
      previewTexture = createTextureFromPath(imagePathBuf);
      loadedPreviewPath = imagePathBuf;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button(tr("btn_browse"), ImVec2(100, 0))) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Select Screen Image or GIF Animation", nullptr,
        GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open",
        GTK_RESPONSE_ACCEPT, nullptr);
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter,
                             "Images and GIF Animations (*.gif, *.png, *.jpg)");
    gtk_file_filter_add_pattern(filter, "*.gif");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
      char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      if (filename) {
        strncpy(imagePathBuf, filename, sizeof(imagePathBuf) - 1);
        imagePathBuf[sizeof(imagePathBuf) - 1] = '\0';
        if (previewTexture != 0)
          glDeleteTextures(1, &previewTexture);
        previewTexture = createTextureFromPath(imagePathBuf);
        loadedPreviewPath = imagePathBuf;
        g_free(filename);
      }
    }
    gtk_widget_destroy(dialog);
  }

  ImGui::SetNextItemWidth(300);
  const char *slotNames[] = {tr("slot_1")};
  ImGui::Combo(tr("tft_slot"), &selectedSlot, slotNames, 1);
  ImGui::TextDisabled("%s", tr("tft_slot_info"));

  // Live Preview of selected image
  if (previewTexture != 0) {
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.85f, 0.85f, 0.90f, 1.0f), "%s",
                       tr("tft_preview_title"));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.35f, 0.40f, 0.50f, 1.0f));
    ImGui::BeginChild("##ImagePreviewFrame", ImVec2(136, 136), true);
    ImGui::Image((ImTextureID)(intptr_t)previewTexture, ImVec2(128, 128));
    ImGui::EndChild();
    ImGui::PopStyleColor();
  }

  // Upload & Progress Bar
  ImGui::Spacing();
  static bool wasUploading = false;
  bool isUploading = GetKeyboard() && GetKeyboard()->isUploading();

  if (wasUploading && !isUploading) {
    if (GetKeyboard()) {
      if (GetKeyboard()->getUploadProgress() >= 1.0f) {
        tftStatusMessage = tr("tft_upload_success");
        refreshGallery();
      } else {
        std::string detail = GetKeyboard()->getUploadStatusDetail();
        if (!detail.empty()) {
          tftStatusMessage = detail;
        }
      }
    }
  }
  wasUploading = isUploading;

  if (isUploading) {
    float progress = GetKeyboard()->getUploadProgress();
    int cur = GetKeyboard()->getUploadChunk();
    int total = GetKeyboard()->getTotalChunks();
    char buf[64];
    snprintf(buf, sizeof(buf), "%d / %d chunks (%.0f%%)", cur, total,
             progress * 100.0f);

    ImGui::TextColored(ImVec4(0.40f, 0.80f, 1.00f, 1.0f), "%s",
                       tr("tft_uploading"));
    ImGui::ProgressBar(progress, ImVec2(350, 26), buf);
  } else {
    if (ImGui::Button(tr("btn_upload_tft"), ImVec2(240, 38))) {
      if (strlen(imagePathBuf) == 0 || !std::filesystem::exists(imagePathBuf)) {
        tftStatusMessage = tr("tft_no_file");
      } else {
        try {
          if (!GetKeyboard()->isConnected()) {
            GetKeyboard()->openHandle();
          }
          GetKeyboard()->uploadImageToLCDAsync(imagePathBuf, 1);
          tftStatusMessage = tr("tft_uploading");
        } catch (const std::exception &e) {
          tftStatusMessage = std::string("Error: ") + e.what();
        }
      }
    }
  }

  if (!tftStatusMessage.empty() && !isUploading) {
    ImGui::Spacing();
    bool isSuccess = (tftStatusMessage.find("Error") == std::string::npos &&
                      tftStatusMessage.find("Por favor") == std::string::npos &&
                      tftStatusMessage.find("Please") == std::string::npos);
    ImVec4 msgColor = isSuccess ? ImVec4(0.35f, 0.85f, 0.45f, 1.00f)
                                : ImVec4(0.95f, 0.35f, 0.35f, 1.00f);
    ImGui::TextColored(msgColor, "%s", tftStatusMessage.c_str());
  }

  ImGui::Spacing();
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.30f, 0.65f, 0.90f, 0.60f));
  ImGui::BeginChild("##TFTViewHint", ImVec2(0, 54), true);
  ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.00f, 1.0f), "💡 %s",
                     tr("tft_view_guide_title"));
  ImGui::TextWrapped("%s", tr("tft_view_guide_desc"));
  ImGui::EndChild();
  ImGui::PopStyleColor();

  ImGui::Dummy(ImVec2(0, 14));
  ImGui::Separator();
  ImGui::Spacing();

  // Section 3: Local Screen Gallery
  if (!galleryScanned) {
    refreshGallery();
  }

  ImGui::TextColored(ImVec4(0.90f, 0.85f, 0.40f, 1.0f), "%s",
                     tr("tft_gallery_title"));
  ImGui::TextDisabled("%s", tr("tft_read_note_msg"));
  ImGui::Spacing();

  if (galleryItems.empty()) {
    ImGui::TextDisabled("%s", tr("gallery_empty"));
  } else {
    for (size_t i = 0; i < galleryItems.size(); ++i) {
      ImGui::PushID(static_cast<int>(i));
      if (galleryItems[i].texture != 0) {
        ImGui::Image((ImTextureID)(intptr_t)galleryItems[i].texture,
                     ImVec2(80, 80));
        ImGui::SameLine();
      }

      ImGui::BeginGroup();
      ImGui::Text("%s", galleryItems[i].filename.c_str());
      if (ImGui::Button(tr("btn_load_preset"), ImVec2(140, 28))) {
        strncpy(imagePathBuf, galleryItems[i].path.c_str(),
                sizeof(imagePathBuf) - 1);
        if (previewTexture != 0)
          glDeleteTextures(1, &previewTexture);
        previewTexture = createTextureFromPath(imagePathBuf);
        loadedPreviewPath = imagePathBuf;
      }
      ImGui::SameLine();
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.20f, 0.20f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.25f, 0.25f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.95f, 0.30f, 0.30f, 1.0f));
      if (ImGui::Button(tr("btn_delete_preset"), ImVec2(90, 28))) {
        try {
          if (std::filesystem::exists(galleryItems[i].path)) {
            std::filesystem::remove(galleryItems[i].path);
          }
          if (loadedPreviewPath == galleryItems[i].path) {
            if (previewTexture != 0) {
              glDeleteTextures(1, &previewTexture);
              previewTexture = 0;
            }
            loadedPreviewPath.clear();
            imagePathBuf[0] = '\0';
          }
          Logger::getInstance().info("Elemento eliminado de la galería: " +
                                     galleryItems[i].filename);
          refreshGallery();
          ImGui::PopStyleColor(3);
          ImGui::EndGroup();
          ImGui::PopID();
          break;
        } catch (const std::exception &e) {
          Logger::getInstance().error("Error eliminando archivo: " +
                                      std::string(e.what()));
        }
      }
      ImGui::PopStyleColor(3);
      ImGui::EndGroup();

      ImGui::Separator();
      ImGui::PopID();
    }
  }

  ImGui::Spacing();
  if (ImGui::Button(tr("btn_refresh_gallery"), ImVec2(160, 28))) {
    refreshGallery();
  }
}
