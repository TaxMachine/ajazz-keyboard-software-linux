//
// i18n.hpp - Internationalization for Ajazz Keyboard Software Linux
// Supports English (Default) and Spanish
// Created by Vixcho on 13/10/26.

#ifndef I18N_HPP
#define I18N_HPP

#include <memory>
#include <string>
#include <unordered_map>

enum class Language { EN = 0, ES = 1 };

class I18n {
public:
  static I18n &getInstance() {
    static I18n instance;
    return instance;
  }

  void setLanguage(Language lang) { currentLang = lang; }

  Language getLanguage() const { return currentLang; }

  void setLanguageCode(const std::string &code) {
    if (code == "es" || code == "ES") {
      currentLang = Language::ES;
    } else {
      currentLang = Language::EN;
    }
  }

  std::string getLanguageCode() const {
    return (currentLang == Language::ES) ? "es" : "en";
  }

  const char *t(const std::string &key) const {
    auto it = translations.find(key);
    if (it != translations.end()) {
      if (currentLang == Language::ES) {
        return it->second.es.c_str();
      } else {
        return it->second.en.c_str();
      }
    }
    return key.c_str();
  }

private:
  struct Entry {
    std::string en;
    std::string es;
  };

  Language currentLang = Language::EN;
  std::unordered_map<std::string, Entry> translations;

  I18n() { initTranslations(); }

  void add(const std::string &key, const std::string &en,
           const std::string &es) {
    translations[key] = {en, es};
  }

  void initTranslations() {
    // Toolbar & Tabs
    add("tab_lighting", "Lighting", "Iluminación");
    add("tab_individual_leds", "Individual LEDs", "LEDs Individuales");
    add("tab_macros", "Macros", "Macros");
    add("tab_tft_screen", "TFT Screen", "Pantalla TFT");
    add("tab_theme", "UI Theme", "Tema UI");
    add("tab_settings", "Settings", "Configuración");
    add("tab_device_status", "Device Status", "Estado USB");
    add("tab_logs", "Logs", "Registros");

    // Status badge
    add("status_usb_cable", "USB Cable", "Cable USB");
    add("status_dongle", "2.4GHz Dongle", "2.4GHz Dongle");
    add("status_disconnected", "Disconnected", "Desconectado");
    add("btn_reconnect", "Reconnect", "Reconectar");

    // Device Status Window
    add("dev_title", "Connection Status & Linux Permissions",
        "Diagnóstico y Estado de Conexión USB / 2.4GHz");
    add("dev_connected", "[CONNECTED] Keyboard ready for communication",
        "[CONECTADO] Teclado listo para comunicación");
    add("dev_disconnected",
        "[DISCONNECTED] Unable to communicate with keyboard",
        "[DESCONECTADO] No se ha podido comunicar con el teclado");
    add("dev_conn_type", "Connection Type: %s", "Tipo de conexión: %s");
    add("dev_type_wireless", "2.4GHz Wireless Dongle",
        "2.4GHz Dongle (Inalámbrico)");
    add("dev_type_wired", "USB Cable (Direct)", "Cable USB (Directo)");
    add("dev_vendor_id", "Vendor ID (VID): 0x0c45 (SONiX / Microdia)",
        "Vendor ID (VID): 0x0c45 (SONiX / Microdia)");
    add("dev_product_id", "Product ID (PID): %s (%s)",
        "Product ID (PID): %s (%s)");
    add("dev_tft_channel", "TFT Screen Channel: %s", "Canal Pantalla TFT: %s");
    add("dev_tft_ready", "Ready & Active (USB Cable display node)",
        "Disponible y activo (cable USB)");
    add("dev_tft_wireless_warn", "Not available over 2.4G (requires USB cable)",
        "No disponible en 2.4G (requiere cable USB)");
    add("dev_tft_not_found", "Not detected (/dev/hidraw display node)",
        "No detectado (/dev/hidraw display)");
    add("btn_retry_conn", "Retry Connection Now",
        "Intentar Conexión Nuevamente");
    add("dev_udev_title", "Linux Device Permissions (udev rules)",
        "Permisos de Dispositivo en Linux (Reglas udev)");
    add("dev_udev_desc",
        "By default, Linux limits raw HID device access (/dev/hidraw*) to "
        "root. To allow regular users to control lighting and upload screen "
        "images without running as root, install the following udev rule:",
        "Por defecto, Linux restringe el acceso directo a dispositivos HID "
        "(/dev/hidraw*) al usuario root. Para permitir que usuarios normales "
        "controlen la iluminación y suban imágenes sin usar sudo, instala la "
        "siguiente regla udev:");
    add("dev_udev_modes",
        "Supports both USB Cable (0x800a / 0x8009) and 2.4GHz Wireless Dongle "
        "(0xfdfd).",
        "Compatible tanto con Cable USB (0x800a / 0x8009) como con Receptor "
        "Inalámbrico 2.4GHz (0xfdfd).");
    add("btn_copy_udev", "Copy Command to Clipboard",
        "Copiar Comando al Portapapeles");
    add("dev_copied_success", "Command copied to clipboard.",
        "Copiado con éxito al portapapeles.");
    add("dev_reconnect_note",
        "After installing the rule, unplug and replug the USB cable or 2.4GHz "
        "dongle.",
        "Una vez instalada la regla udev, desconecta y vuelve a conectar el "
        "cable USB o el receptor 2.4GHz.");

    // TFT Screen Window
    add("tft_title", "TFT Display & RTC Clock Control",
        "Control de Pantalla TFT y Reloj");
    add("tft_clock_section",
        "RTC Clock Synchronization:", "Sincronización de Hora y Fecha:");
    add("tft_clock_desc",
        "Sync your PC system clock with the keyboard's internal real-time "
        "clock.",
        "Sincroniza el reloj del sistema de tu PC con la pantalla TFT del "
        "teclado.");
    add("tft_sys_time", "System Local Time: %s", "Hora local del sistema: %s");
    add("btn_sync_clock", "Sync Clock with Keyboard",
        "Sincronizar Reloj con el Teclado");
    add("tft_clock_success", "Clock synchronized successfully.",
        "Reloj sincronizado exitosamente con el teclado.");
    add("tft_upload_section", "LCD Image & Animated GIF Upload (128x128 px):",
        "Carga de Imágenes y GIF (128x128 px):");
    add("tft_upload_desc",
        "Supports PNG, JPG, and animated GIF files. Images are automatically "
        "cropped and rescaled to 128x128 with bilinear filtering.",
        "Soporta imágenes PNG, JPG y animaciones GIF. Las imágenes se ajustan "
        "automáticamente a 128x128 con filtro bilineal.");
    add("tft_hardware_warn_title", "Hardware Requirement: USB Cable Required",
        "Aviso de Hardware: Requiere Cable USB");
    add("tft_hardware_warn_msg",
        "To upload images or GIF animations to the TFT screen, the keyboard "
        "must be connected directly via USB cable with the back switch in USB "
        "mode (wireless 2.4G does not support bulk display transfers).",
        "Para transferir imágenes o animaciones GIF a la pantalla TFT, el "
        "teclado debe estar conectado directamente por cable USB con el "
        "interruptor trasero en modo USB (el modo inalámbrico 2.4G no soporta "
        "transferencia de pantalla).");
    add("tft_channel_ready",
        "[OK] TFT screen channel is ready for USB cable transfer.",
        "[OK] Canal de pantalla TFT preparado para transferencia por cable "
        "USB.");
    add("tft_file_path", "File path (PNG, JPG, GIF)",
        "Ruta del archivo (PNG, JPG, GIF)");
    add("btn_browse", "Browse...", "Examinar...");
    add("tft_slot", "Storage Slot", "Ranura de Almacenamiento");
    add("slot_1", "Slot 1 (Hardware Screen)", "Ranura 1 (Memoria de Pantalla)");
    add("tft_slot_info",
        "Note: The AK820 Pro hardware features 1 internal flash slot for "
        "custom animations. You can save multiple presets in the local gallery "
        "below.",
        "Nota: El hardware del AK820 Pro dispone de 1 ranura en memoria para "
        "tu imagen/animación. Puedes guardar múltiples imágenes en tu galería "
        "local.");
    add("btn_upload_tft", "Upload to TFT Screen", "Subir a Pantalla TFT");
    add("tft_preview_title", "Image & Frame Preview (128x128):",
        "Vista Previa de la Imagen (128x128):");
    add("tft_uploading", "Uploading chunks to screen...",
        "Transfiriendo bloques a la pantalla...");
    add("tft_upload_success",
        "Upload complete! Image saved to TFT screen. Press FN + Del on your "
        "keyboard to display it.",
        "¡Carga completada! Imagen guardada en la pantalla TFT. Presiona FN + "
        "Del en tu teclado para mostrarla.");
    add("tft_view_guide_title",
        "How to view your animation on the keyboard screen:",
        "Cómo ver la animación en la pantalla del teclado:");
    add("tft_view_guide_desc",
        "Press FN + Del on your keyboard to switch between the clock screen "
        "and your custom animation. You can also press and hold the rotary "
        "knob to open the TFT screen menu.",
        "Presiona el atajo FN + Del en tu teclado para alternar entre el reloj "
        "y tu animación. También puedes mantener presionada la rueda giratoria "
        "(Knob) para abrir el menú TFT.");
    add("tft_upload_failed", "Upload failed: %s", "Error al subir imagen: %s");
    add("tft_progress", "Progress: %d / %d chunks (%.0f%%)",
        "Progreso: %d / %d bloques (%.0f%%)");
    add("tft_no_file", "Please select a valid image or GIF file.",
        "Por favor selecciona o introduce la ruta de un archivo existente.");
    add("tft_gallery_title",
        "Local Image & GIF Gallery:", "Galería Local de Imágenes y GIFs:");
    add("tft_read_note_title",
        "Why does the keyboard not send back stored images?",
        "¿Por qué el teclado no devuelve las imágenes guardadas?");
    add("tft_read_note_msg",
        "The keyboard's MCU firmware writes image data directly to on-board "
        "SPI flash memory and does not implement a USB read-back command. The "
        "gallery below preserves your uploaded images locally so you can "
        "preview and re-upload them at any time.",
        "El firmware del microcontrolador escribe los datos directamente en la "
        "memoria flash SPI interna y no incluye un comando USB de lectura. La "
        "galería inferior guarda tus imágenes localmente para que puedas "
        "previsualizarlas y volver a cargarlas en cualquier momento.");
    add("btn_load_preset", "Apply This Image", "Cargar Esta Imagen");
    add("btn_delete_preset", "Delete", "Eliminar");

    // Settings Window
    add("settings_title", "Application Settings & Preferences",
        "Configuración de la Aplicación y Preferencias");
    add("settings_lang_label",
        "Application Language (Idioma):", "Idioma de la Aplicación:");
    add("settings_lang_en", "English (Default)", "English (Inglés)");
    add("settings_lang_es", "Español (Spanish)", "Español");
    add("settings_startup", "Startup Behavior:", "Comportamiento de Inicio:");
    add("settings_autoconnect",
        "Automatically scan and connect to keyboard on startup",
        "Escanear y conectar automáticamente al teclado al iniciar");
    add("settings_config_path",
        "Configuration File Path:", "Ruta del Archivo de Configuración:");
    add("btn_copy_path", "Copy Path", "Copiar Ruta");
    add("btn_save_settings", "Save Configuration", "Guardar Configuración");
    add("settings_saved_msg", "Configuration saved successfully to disk.",
        "Configuración guardada exitosamente en el disco.");
    add("settings_changelog_title", "Version History & Patch Notes",
        "Historial de Versiones y Parches");
    add("settings_changelog_desc",
        "Record of releases, Linux HID protocol adaptations, and driver "
        "improvements.",
        "Registro de versiones, adaptaciones del protocolo HID en Linux y "
        "mejoras del controlador.");
    add("changelog_v002_title",
        "Version 0.0.2 - Hotfixes, TFT Reliability & Quality of Life (Latest)",
        "Versión 0.0.2 - Hotfixes, Estabilidad TFT y Mejoras de Calidad de "
        "Vida (Actual)");
    add("changelog_v001_title",
        "Version 0.0.1 - Initial Linux Port & Protocol Adaptations (Fork)",
        "Versión 0.0.1 - Fork Inicial y Adaptaciones del Protocolo Linux "
        "(Base)");

    // Lighting Window
    add("mode_title", "RGB Lighting Configuration",
        "Configuración de Iluminación RGB");
    add("mode_lighting_mode", "Lighting Mode:", "Modo de Iluminación:");
    add("mode_rainbow", "Rainbow Mode (Multicolor)",
        "Modo Arcoíris (Multicolor)");
    add("mode_static_color", "Static Color", "Color Estático");
    add("mode_brightness", "Brightness (0-5)", "Nivel de Brillo (0-5)");
    add("mode_speed", "Animation Speed (0-5)", "Velocidad de Animación (0-5)");
    add("mode_direction", "Animation Direction:", "Dirección de la Animación:");
    add("mode_sleep_time",
        "Auto-Sleep Time:", "Tiempo de Suspensión (Auto-Sleep):");
    add("btn_apply_lighting", "Apply Lighting", "Aplicar Iluminación");
    add("mode_applied_success", "RGB lighting applied and saved to keyboard!",
        "¡Iluminación RGB aplicada y guardada en el teclado!");
    add("btn_turn_on_leds", "Turn On LEDs (Rainbow)",
        "Encender LEDs (Arcoíris)");
    add("btn_turn_off_leds", "Turn Off LEDs", "Apagar LEDs");
    add("dir_left", "Left", "Izquierda");
    add("dir_up", "Up", "Arriba");
    add("dir_down", "Down", "Abajo");
    add("dir_right", "Right", "Derecha");

    // Lighting Mode names
    add("lm_off", "Off", "Apagado");
    add("lm_static", "Static", "Estático");
    add("lm_single_on", "Single On", "Encendido Simple");
    add("lm_single_off", "Single Off", "Apagado Simple");
    add("lm_glittering", "Glittering", "Brillante");
    add("lm_falling", "Falling", "Caída");
    add("lm_colourful", "Colourful", "Colorido");
    add("lm_breath", "Breathing", "Respiración");
    add("lm_spectrum", "Spectrum Cycle", "Espectro");
    add("lm_outward", "Outward Ripple", "Expansión");
    add("lm_scrolling", "Scrolling", "Desplazamiento");
    add("lm_rolling", "Rolling", "Rodante");
    add("lm_rotating", "Rotating", "Giratorio");
    add("lm_explode", "Explode", "Explosión");
    add("lm_launch", "Launch", "Lanzamiento");
    add("lm_ripples", "Ripples", "Ondas");
    add("lm_flowing", "Flowing", "Fluido");
    add("lm_pulsating", "Pulsating", "Pulsante");
    add("lm_tilt", "Tilt", "Inclinación");
    add("lm_shuttle", "Shuttle", "Lanzadera");
    add("lm_custom", "Custom Per-Key", "Personalizado (Por Tecla)");

    // Individual LEDs
    add("leds_title", "Per-Key Individual RGB Lighting Editor",
        "Editor de LEDs Individuales (Per-Key RGB)");
    add("leds_desc",
        "Click on any key to customize its color, or apply a solid color to "
        "all keys.",
        "Haz clic en cualquier tecla para personalizar su color, o aplica un "
        "color sólido a todas.");
    add("leds_brush_color", "Brush Color:", "Color del Pincel:");
    add("leds_btn_wasd", "WASD", "WASD");
    add("leds_btn_arrows", "Arrows", "Flechas");
    add("leds_btn_all", "All", "Todo");
    add("leds_btn_off", "Turn Off All", "Apagar Todo");
    add("btn_apply_individual_leds", "Apply to Keyboard", "Aplicar al Teclado");
    add("leds_success", "Per-key LEDs sent successfully to keyboard.",
        "LEDs individuales enviados exitosamente al teclado.");
    add("leds_error", "Error sending LEDs: ", "Error enviando LEDs: ");

    // Macros
    add("macros_title", "Macro Manager & Recorder",
        "Gestor y Grabador de Macros");
    add("macros_saved", "Saved Macros:", "Macros Guardadas:");
    add("macros_btn_new", "+ New Macro", "+ Nueva Macro");
    add("macros_btn_delete", "Delete", "Eliminar");
    add("macros_settings", "Macro Settings:", "Configuración de Macro:");
    add("macros_name", "Name", "Nombre");
    add("macros_trigger", "Trigger Key", "Tecla Disparadora");
    add("macros_save_meta", "Save Metadata", "Guardar Metadatos");
    add("macros_actions_seq",
        "Actions Sequence (%zu steps):", "Secuencia de Acciones (%zu pasos):");
    add("macros_press", "Press", "Presionar");
    add("macros_release", "Release", "Soltar");
    add("macros_key", "Key##Add", "Tecla##Add");
    add("macros_delay", "Delay (ms)", "Delay (ms)");
    add("macros_btn_add_pulse", "Add Keypress", "Agregar Pulsación");
    add("macros_btn_clear", "Clear Actions", "Limpiar Acciones");

    // UI Theme
    add("theme_title", "Visual Theme Customization",
        "Personalización del Tema Visual");
    add("theme_window_rounding", "Window Rounding", "Redondeo de Ventana");
    add("theme_frame_rounding", "Frame Rounding", "Redondeo de Marcos");
    add("theme_padding_x", "Frame Padding (X)", "Espaciado de Marcos (X)");
    add("theme_padding_y", "Frame Padding (Y)", "Espaciado de Marcos (Y)");

    // Logs Window
    add("logs_title", "Event Logs & Diagnostics",
        "Registro de Eventos y Diagnóstico de Errores (Logs)");
    add("btn_copy_all_logs", "Copy All Logs", "Copiar Todos los Logs");
    add("btn_clear_logs", "Clear Logs", "Limpiar Logs");
    add("logs_auto_scroll", "Auto-scroll", "Desplazamiento Automático");
    add("logs_copied_msg", "Logs copied to clipboard.",
        "Logs copiados al portapapeles con éxito.");
    add("logs_empty", "No events recorded yet.",
        "No hay eventos registrados aún.");
    add("btn_refresh_gallery", "Refresh Gallery", "Refrescar Galería");
    add("gallery_empty",
        "No images in local gallery yet. Upload an image to save it here.",
        "Aún no hay imágenes en la galería local. Sube una imagen para "
        "guardarla aquí.");
  }
};

inline const char *tr(const std::string &key) {
  return I18n::getInstance().t(key);
}

#endif // I18N_HPP
