//
// configwindow.cpp
// Application Settings & Configuration Management
// Created by Vixcho on 13/10/26.
//

#include "../../constants.hpp"
#include "../../utils/i18n.hpp"
#include "../../utils/logger.hpp"
#include "../app.hpp"
#include <filesystem>
#include <imgui.h>
#include <string>
#include <vector>

static bool settingsSaved = false;

struct PatchEntry {
  std::string en;
  std::string es;
};

static const std::vector<PatchEntry> patchesV002 = {
    {"Added this->handshake() after START_PKT, cmdImage, and SAVE_PKT so the keyboard flash memory validates and commits transferred frames.",
     "Se añadió this->handshake() tras START_PKT, cmdImage y SAVE_PKT para que la memoria flash del teclado valide y fije los fotogramas transferidos."},
    {"Added reentrant thread-safety lock (std::recursive_mutex) preventing race conditions during asynchronous image uploads while GUI reads progress.",
     "Se agregó bloqueo de seguridad reentrante (std::recursive_mutex) para evitar conflictos de hilos durante la subida asíncrona mientras la GUI consulta el progreso."},
    {"Normalized animated GIF frame delay calculation (delayMs / 2) according to the microcontroller firmware's 2 ms timer units.",
     "Se normalizó el cálculo del tiempo de fotogramas en GIFs animados (delayMs / 2) acorde a las unidades de 2 ms del firmware."},
    {"Clamped display slot selector to Slot 1 (the single physical flash slot supported by AK820 Pro hardware for custom animations).",
     "Se ajustó el selector a la Ranura 1 (la única soportada físicamente por el hardware para animaciones personalizadas)."},
    {"Added a permanent guidance card and post-upload notification reminding users to press FN + Del or hold the rotary knob to view the screen animation.",
     "Se añadió un recuadro explicativo permanente y mensajes tras la subida recordando pulsar FN + Del o mantener el Knob para ver la imagen en la pantalla del teclado."},
    {"Added interactive Delete button to remove cached images and GIFs from local gallery (~/.config/ak820proLED/gallery/) and release textures.",
     "Se añadió un botón en la galería local para poder borrar cosas de la galería de imágenes y GIFs (~/.config/ak820proLED/gallery/), liberando memoria de texturas y espacio en disco."},
    {"Resolved GTK/GLib setlocale console warnings in src/main.cpp using a clean GLib log filter.",
     "Corrección de advertencia GTK en src/main.cpp: se eliminó el aviso de consola de setlocale / GLib mediante un filtro limpio."}
};

static const std::vector<PatchEntry> patchesV001 = {
    {"Initial fork from TaxMachine (https://github.com/TaxMachine/ajazz-keyboard-software-linux) with adaptations for Arch Linux and modern distributions.",
     "Fork inicial basado en TaxMachine (https://github.com/TaxMachine/ajazz-keyboard-software-linux) con adaptaciones profundas para Arch Linux y kernels modernos."},
    {"Adjusted packet length to 65 bytes (Report ID 0x00 + 64 bytes payload) for unnumbered reports in the Linux kernel.",
     "Ajuste de longitud de paquetes a 65 bytes (Report ID 0x00 + 64 bytes payload) para reportes sin numerar en el kernel Linux."},
    {"Removed the erroneous FINISH_PKT (0x04, 0xf0) command that was turning off LEDs.",
     "Se eliminó el comando erróneo FINISH_PKT (0x04, 0xf0) que provocaba que los LEDs se apagaran."},
    {"Added automatic detection and opening of both direct wired USB (0x800a / 0x8009) and 2.4 GHz wireless dongle (0xfdfd), allowing keyboard operation regardless of physical switch position (resolving Arch Linux KDE issues).",
     "Detección y apertura automática tanto de cable USB directo (0x800a / 0x8009) como del receptor inalámbrico 2.4 GHz (0xfdfd), permitiendo operar el teclado sin importar la posición del interruptor físico (resolviendo problemas en Arch Linux KDE)."},
    {"Added dynamic English and Spanish language system in i18n.hpp, with English as default and full Spanish support.",
     "Sistema dinámico de idiomas en i18n.hpp (Inglés por defecto y soporte completo para Español) con cambio en caliente."},
    {"Added language and settings persistence saved automatically to ~/.config/ak820proLED/config.json.",
     "Persistencia de configuración guardada automáticamente en ~/.config/ak820proLED/config.json."},
    {"Added timestamped event logging for INFO, WARNING, ERROR, and SUCCESS messages.",
     "Registro de eventos con marcas de tiempo para mensajes INFO, WARNING, ERROR y SUCCESS."},
    {"Added UI buttons to copy the log to clipboard, clear the console, and toggle auto-scroll.",
     "Botones en la interfaz para copiar el registro al portapapeles, limpiar la consola y activar/desactivar auto-scroll."},
    {"Added automatic center-cropping and resizing for non-square images to 128x128 px using bilinear interpolation with GdkPixbuf.",
     "Recorte centrado y redimensionado automático para imágenes no cuadradas a 128x128 px con interpolación bilineal usando GdkPixbuf."},
    {"Added conversion to RGB565 Little-Endian (16-bit packed pixel: ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3), stored as low byte then high byte).",
     "Conversión a RGB565 Little-Endian (píxel empaquetado de 16 bits: ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3), almacenado como byte bajo y luego byte alto)."},
    {"Added animated GIF support using GdkPixbufAnimationIter to parse frames and frame delay intervals.",
     "Soporte para GIFs animados usando GdkPixbufAnimationIter para procesar fotogramas e intervalos de delay."},
    {"Added 256-byte header packing for GIFs defining up to 128 frames and delays in 10 ms multiples.",
     "Empaquetado de cabecera de 256 bytes para GIFs definiendo hasta 128 fotogramas y retardos en múltiplos de 10 ms."},
    {"Configured display data transmission through keyboard interface 2 (UsagePage 0xFF68) via /dev/hidraw in wired USB mode.",
     "Configuración de transmisión de datos de pantalla a través de la interfaz 2 (UsagePage 0xFF68) mediante /dev/hidraw en modo cable USB."},
    {"Implemented start protocol packet (0x04, 0x72) specifying target memory slot (Slot 1, 2, or 3) and total block count.",
     "Implementación del paquete de protocolo de inicio (0x04, 0x72) especificando ranura de memoria de destino y conteo total de bloques."},
    {"Added data fragmentation into 4096-byte chunks (IMAGE_CHUNK_SIZE) for up to ~4.1 MB with flow control and ACK verification using poll()/read().",
     "Fragmentación de datos en bloques de 4096 bytes (IMAGE_CHUNK_SIZE) para hasta ~4.1 MB con control de flujo y verificación ACK mediante poll()/read()."},
    {"Added SAVE_PKT (0x04, 0x02) command to permanently write image/GIF data to internal flash memory.",
     "Comando SAVE_PKT (0x04, 0x02) para escribir de forma permanente los datos de imagen/GIF en la memoria flash interna."},
    {"Added real-time percentage progress bar in the UI during uploads.",
     "Barra de progreso porcentual en tiempo real en la interfaz de usuario durante las subidas."},
    {"Added automatic caching of uploaded images to local gallery at ~/.config/ak820proLED/gallery/.",
     "Guardado automático de imágenes subidas en la galería local en ~/.config/ak820proLED/gallery/."}
};

void AjazzGUI::ConfigWindow() {
  ImGui::TextColored(ImVec4(0.35f, 0.70f, 1.00f, 1.00f), "%s",
                     tr("settings_title"));
  ImGui::Separator();
  ImGui::Spacing();

  // Section 1: Language
  ImGui::Text("%s", tr("settings_lang_label"));
  int currentLangIdx =
      (I18n::getInstance().getLanguage() == Language::ES) ? 1 : 0;
  const char *langOptions[] = {tr("settings_lang_en"), tr("settings_lang_es")};

  ImGui::SetNextItemWidth(260);
  if (ImGui::Combo("##LanguageSelector", &currentLangIdx, langOptions, 2)) {
    if (currentLangIdx == 1) {
      I18n::getInstance().setLanguage(Language::ES);
      if (GetConfig())
        GetConfig()->language = "es";
    } else {
      I18n::getInstance().setLanguage(Language::EN);
      if (GetConfig())
        GetConfig()->language = "en";
    }
    Logger::getInstance().info("Language set to: " +
                               I18n::getInstance().getLanguageCode());
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // Section 2: Startup & Connection
  ImGui::Text("%s", tr("settings_startup"));
  if (GetConfig()) {
    ImGui::Checkbox(tr("settings_autoconnect"), &GetConfig()->auto_connect);
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // Section 3: Configuration File
  ImGui::Text("%s", tr("settings_config_path"));
  std::string configPath = GetConfig() ? GetConfig()->getConfigFilePath()
                                       : "~/.config/ak820proLED/config.json";
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.09f, 0.11f, 1.00f));
  ImGui::BeginChild("##ConfigPathBox", ImVec2(0, 42), true);
  ImGui::TextUnformatted(configPath.c_str());
  ImGui::EndChild();
  ImGui::PopStyleColor();

  ImGui::Spacing();
  if (ImGui::Button(tr("btn_copy_path"), ImVec2(160, 32))) {
    ImGui::SetClipboardText(configPath.c_str());
  }

  ImGui::Dummy(ImVec2(0, 16));
  ImGui::Separator();
  ImGui::Spacing();

  // Section 4: Save Configuration
  if (ImGui::Button(tr("btn_save_settings"), ImVec2(240, 40))) {
    if (GetConfig()) {
      GetConfig()->saveConfig();
      settingsSaved = true;
      Logger::getInstance().success("Configuration saved to: " + configPath);
    }
  }

  if (settingsSaved) {
    ImGui::SameLine(0, 15);
    ImGui::TextColored(ImVec4(0.35f, 0.85f, 0.45f, 1.00f), "%s",
                       tr("settings_saved_msg"));
  }

  ImGui::Dummy(ImVec2(0, 16));
  ImGui::Separator();
  ImGui::Spacing();

  // Section 5: Version History & Patch Notes
  ImGui::TextColored(ImVec4(0.35f, 0.70f, 1.00f, 1.00f), "%s",
                     tr("settings_changelog_title"));
  ImGui::TextDisabled("%s", tr("settings_changelog_desc"));
  ImGui::Spacing();

  bool isEs = (I18n::getInstance().getLanguage() == Language::ES);

  // v0.0.2 Collapsing Header (Open by default)
  ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.16f, 0.32f, 0.48f, 0.85f));
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.22f, 0.42f, 0.62f, 0.95f));
  ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.26f, 0.48f, 0.70f, 1.00f));
  if (ImGui::CollapsingHeader(tr("changelog_v002_title"), ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::PopStyleColor(3);
    ImGui::Indent(12.0f);
    ImGui::Spacing();
    for (const auto &item : patchesV002) {
      ImGui::Bullet();
      ImGui::TextWrapped("%s", isEs ? item.es.c_str() : item.en.c_str());
      ImGui::Spacing();
    }
    ImGui::Unindent(12.0f);
  } else {
    ImGui::PopStyleColor(3);
  }

  ImGui::Spacing();

  // v0.0.1 Collapsing Header
  ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.14f, 0.17f, 0.22f, 0.85f));
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.20f, 0.24f, 0.30f, 0.95f));
  ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.24f, 0.30f, 0.38f, 1.00f));
  if (ImGui::CollapsingHeader(tr("changelog_v001_title"))) {
    ImGui::PopStyleColor(3);
    ImGui::Indent(12.0f);
    ImGui::Spacing();
    for (const auto &item : patchesV001) {
      ImGui::Bullet();
      ImGui::TextWrapped("%s", isEs ? item.es.c_str() : item.en.c_str());
      ImGui::Spacing();
    }
    ImGui::Unindent(12.0f);
  } else {
    ImGui::PopStyleColor(3);
  }

  ImGui::Spacing();
}
