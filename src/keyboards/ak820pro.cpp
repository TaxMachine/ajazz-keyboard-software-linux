//
// Created by taxmachine on 16/02/25.
// Modified by Vixcho on 13/10/26.
//

#include "ak820pro.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <filesystem>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <iostream>
#include <lodepng.h>
#include <poll.h>
#include <sstream>
#include <thread>
#include <unistd.h>

#include "../utils/logger.hpp"
#include "../utils/utils.hpp"

static uint8_t START_PKT[PACKET_LENGTH] = {
    COMMAND_PREFIX, START_COMMAND, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static uint8_t SAVE_PKT[PACKET_LENGTH] = {
    COMMAND_PREFIX, SAVE_COMMAND, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t FINISH_PKT[PACKET_LENGTH] = {
    COMMAND_PREFIX, FINISH_COMMAND, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static uint8_t START_MODE_PKT[PACKET_LENGTH] = {
    COMMAND_PREFIX, MODE_COMMAND, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static uint8_t START_SLEEP_PKT[PACKET_LENGTH] = {
    COMMAND_PREFIX, 0x17, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static uint8_t START_TIME_PKT[PACKET_LENGTH] = {
    COMMAND_PREFIX, TIME_COMMAND, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
static std::vector<uint8_t> processImageOrGif(const std::string &path) {
  GError *error = nullptr;
  GdkPixbufAnimation *anim =
      gdk_pixbuf_animation_new_from_file(path.c_str(), &error);
  if (!anim) {
    std::string err = error ? error->message : "Desconocido";
    if (error)
      g_error_free(error);
    throw std::runtime_error("No se pudo cargar la imagen: " + err);
  }

  constexpr int TFT_W = TFT_WIDTH;
  constexpr int TFT_H = TFT_HEIGHT;
  constexpr size_t FRAME_BYTES = TFT_FRAME_BYTES;
  constexpr size_t HEADER_BYTES = TFT_HEADER_BYTES;

  auto convertPixbufToRGB565 = [](GdkPixbuf *pixbuf) -> std::vector<uint8_t> {
    int orig_w = gdk_pixbuf_get_width(pixbuf);
    int orig_h = gdk_pixbuf_get_height(pixbuf);
    GdkPixbuf *cropped = nullptr;
    if (orig_w != orig_h) {
      int side = std::min(orig_w, orig_h);
      int src_x = (orig_w - side) / 2;
      int src_y = (orig_h - side) / 2;
      cropped = gdk_pixbuf_new_subpixbuf(pixbuf, src_x, src_y, side, side);
    }
    GdkPixbuf *src = cropped ? cropped : pixbuf;
    GdkPixbuf *scaled =
        gdk_pixbuf_scale_simple(src, TFT_W, TFT_H, GDK_INTERP_BILINEAR);
    if (cropped)
      g_object_unref(cropped);
    if (!scaled)
      scaled = GDK_PIXBUF(g_object_ref(src));

    int n_channels = gdk_pixbuf_get_n_channels(scaled);
    int rowstride = gdk_pixbuf_get_rowstride(scaled);
    const guchar *pixels = gdk_pixbuf_get_pixels(scaled);

    std::vector<uint8_t> out(FRAME_BYTES, 0);
    for (int y = 0; y < TFT_H; ++y) {
      const guchar *row = pixels + y * rowstride;
      for (int x = 0; x < TFT_W; ++x) {
        uint8_t r = row[x * n_channels];
        uint8_t g = row[x * n_channels + 1];
        uint8_t b = row[x * n_channels + 2];
        uint16_t pixel = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        int idx = (y * TFT_W + x) * 2;
        out[idx] = static_cast<uint8_t>(pixel & 0xFF);
        out[idx + 1] = static_cast<uint8_t>((pixel >> 8) & 0xFF);
      }
    }
    g_object_unref(scaled);
    return out;
  };

  struct FrameData {
    std::vector<uint8_t> rgb565;
    int delayUnits;
  };
  std::vector<FrameData> frames;

  if (gdk_pixbuf_animation_is_static_image(anim)) {
    GdkPixbuf *staticPix = gdk_pixbuf_animation_get_static_image(anim);
    if (staticPix) {
      frames.push_back({convertPixbufToRGB565(staticPix), 1});
    }
  } else {
    GTimeVal timeVal = {0, 0};
    GdkPixbufAnimationIter *iter =
        gdk_pixbuf_animation_get_iter(anim, &timeVal);
    int maxFrames = 150;
    while (iter && (int)frames.size() < maxFrames) {
      GdkPixbuf *curPix = gdk_pixbuf_animation_iter_get_pixbuf(iter);
      int delayMs = gdk_pixbuf_animation_iter_get_delay_time(iter);
      if (delayMs <= 0)
        delayMs = 50;
      int delayUnits = std::max(1, std::min(255, delayMs / 2));
      if (curPix) {
        frames.push_back({convertPixbufToRGB565(curPix), delayUnits});
      }
      timeVal.tv_usec += delayMs * 1000;
      if (timeVal.tv_usec >= 1000000) {
        timeVal.tv_sec += timeVal.tv_usec / 1000000;
        timeVal.tv_usec %= 1000000;
      }
      gdk_pixbuf_animation_iter_advance(iter, &timeVal);
      if (gdk_pixbuf_animation_iter_on_currently_loading_frame(iter)) {
        break;
      }
    }
    if (iter)
      g_object_unref(iter);
  }
  g_object_unref(anim);

  if (frames.empty()) {
    throw std::runtime_error("No se pudieron extraer fotogramas de la imagen.");
  }

  size_t nFrames = frames.size();
  std::vector<uint8_t> payload(HEADER_BYTES + nFrames * FRAME_BYTES, 0xFF);
  payload[0] = static_cast<uint8_t>(nFrames);
  for (size_t i = 0; i < nFrames; ++i) {
    payload[1 + i] = static_cast<uint8_t>(frames[i].delayUnits);
    memcpy(payload.data() + HEADER_BYTES + i * FRAME_BYTES,
           frames[i].rgb565.data(), FRAME_BYTES);
  }
  return payload;
}
#pragma GCC diagnostic pop

AK820Pro::AK820Pro(const uint16_t vid, const uint16_t pid) {
  hid_init();
  this->vid = vid;
  this->pid = pid;
  this->openHandle();
}

AK820Pro::~AK820Pro() {
  this->closeHandle();
  hid_exit();
}

bool AK820Pro::openHandle() {
  std::lock_guard<std::recursive_mutex> lock(this->hidMutex);
  if (this->handle || this->wirelessHandle) {
    return true;
  }

  Logger::getInstance().info("Escaneando dispositivos HID (VID 0x0c45)...");
  struct hid_device_info *devs = hid_enumerate(this->vid, 0);
  if (!devs) {
    this->lastErrorMessage = "No se encontraron dispositivos con VID 0x0c45";
    Logger::getInstance().warning(this->lastErrorMessage);
    return false;
  }

  std::string cmdPathWired;
  std::string dispPathWired;
  std::string cmdPathWireless;
  uint16_t foundPid = 0;

  for (struct hid_device_info *cur = devs; cur != nullptr; cur = cur->next) {
    if (cur->vendor_id != AJAZZ_VID || !cur->path) {
      continue;
    }

    bool isWired = (cur->product_id == AK820PRO_PID ||
                    cur->product_id == AK820PRO_PID_ALT);
    bool isWireless = (cur->product_id == AK820PRO_PID_WIRELESS);

    if (isWired) {
      foundPid = cur->product_id;
      if (cur->usage_page == 0xff13 || cur->interface_number == 3) {
        cmdPathWired = cur->path;
      } else if (cur->usage_page == 0xff68 || cur->interface_number == 2) {
        dispPathWired = cur->path;
      }
    } else if (isWireless) {
      if (cur->interface_number == 4 || cur->usage_page == 0xff59) {
        cmdPathWireless = cur->path;
      } else if (cmdPathWireless.empty() &&
                 (cur->interface_number == 3 || cur->usage_page == 0xff60)) {
        cmdPathWireless = cur->path;
      }
    }
  }

  // Open Wired (USB cable) if present
  if (!cmdPathWired.empty()) {
    this->handle = hid_open_path(cmdPathWired.c_str());
    if (this->handle) {
      this->activePath = cmdPathWired;
      this->pid = foundPid;
      this->wirelessMode = false;
      this->lastErrorMessage.clear();
      std::stringstream ss;
      ss << std::hex << foundPid;
      Logger::getInstance().success("Conectado a Cable USB (PID 0x" + ss.str() +
                                    ") en " + this->activePath);

      if (!dispPathWired.empty()) {
        if (this->dispFd >= 0) {
          ::close(this->dispFd);
          this->dispFd = -1;
        }
        this->dispFd = ::open(dispPathWired.c_str(), O_RDWR | O_CLOEXEC);
        if (this->dispFd >= 0) {
          this->dispPath = dispPathWired;
          Logger::getInstance().success("Canal de pantalla TFT abierto en " +
                                        this->dispPath);
        } else {
          Logger::getInstance().warning(
              "No se pudo abrir canal TFT en " + dispPathWired +
              " (errno: " + std::to_string(errno) + ")");
        }
      }
    }
  }

  // Also open Wireless 2.4G dongle if present (covers both USB switch
  // positions)
  if (!cmdPathWireless.empty()) {
    this->wirelessHandle = hid_open_path(cmdPathWireless.c_str());
    if (this->wirelessHandle) {
      this->lastErrorMessage.clear();
      Logger::getInstance().success("Canal 2.4GHz Dongle (0xfdfd) abierto en " +
                                    cmdPathWireless);
      if (!this->handle) {
        this->activePath = cmdPathWireless;
        this->pid = AK820PRO_PID_WIRELESS;
        this->wirelessMode = true;
      }
    }
  }

  hid_free_enumeration(devs);
  if (this->handle || this->wirelessHandle) {
    return true;
  }

  this->lastErrorMessage =
      "No se pudo abrir el dispositivo. Comprueba permisos udev y conexión.";
  Logger::getInstance().error(this->lastErrorMessage);
  return false;
}

void AK820Pro::closeHandle() {
  std::lock_guard<std::recursive_mutex> lock(this->hidMutex);
  if (this->dispFd >= 0) {
    ::close(this->dispFd);
    this->dispFd = -1;
    this->dispPath.clear();
  }
  if (this->handle) {
    hid_close(this->handle);
    this->handle = nullptr;
  }
  if (this->wirelessHandle) {
    hid_close(this->wirelessHandle);
    this->wirelessHandle = nullptr;
  }
  Logger::getInstance().info("Conexión con el teclado cerrada.");
}

bool AK820Pro::isConnected() const {
  return (this->handle != nullptr || this->wirelessHandle != nullptr);
}

bool AK820Pro::isWireless() const {
  return (this->handle == nullptr && this->wirelessHandle != nullptr);
}

bool AK820Pro::isDisplayAvailable() const { return (this->dispFd >= 0); }

std::string AK820Pro::getConnectionStatus() const {
  if (!this->handle && !this->wirelessHandle) {
    return "Desconectado";
  }
  if (this->handle && this->wirelessHandle) {
    return "Conectado (Cable USB + Dongle 2.4G)";
  }
  return this->handle ? "Conectado (Cable USB)" : "Conectado (2.4GHz Dongle)";
}

std::string AK820Pro::getLastError() const { return this->lastErrorMessage; }

bool AK820Pro::sendPacket(const uint8_t *payload64) const {
  if (!this->handle && !this->wirelessHandle)
    return false;

  uint8_t report[65] = {0};
  report[0] = 0x00; // Report ID 0x00 for unnumbered reports
  memcpy(report + 1, payload64, PACKET_LENGTH);

  bool ok = false;
  if (this->handle) {
    int res = hid_send_feature_report(this->handle, report, 65);
    if (res >= 0)
      ok = true;
  }
  if (this->wirelessHandle) {
    int res = hid_send_feature_report(this->wirelessHandle, report, 65);
    if (res >= 0)
      ok = true;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(40));
  return ok;
}

void AK820Pro::handshake() const {
  uint8_t report[65] = {0};
  report[0] = 0x00;
  if (this->handle) {
    hid_get_feature_report(this->handle, report, 65);
  }
  if (this->wirelessHandle) {
    hid_get_feature_report(this->wirelessHandle, report, 65);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(40));
}

void AK820Pro::setColor(const uint8_t r, const uint8_t g,
                        const uint8_t b) const {
  this->setMode(STATIC, r, g, b, false, MAX_BRIGHTNESS, MAX_SPEED, LEFT);
}

void AK820Pro::setModeUnlocked(const LightingMode mode, const uint8_t r,
                               const uint8_t g, const uint8_t b,
                               const bool rainbow, const int brightness,
                               const int speed,
                               const Direction direction) const {
  if (!this->handle && !this->wirelessHandle &&
      !const_cast<AK820Pro *>(this)->openHandle()) {
    std::string err =
        "Dispositivo HID no disponible: " + this->lastErrorMessage;
    Logger::getInstance().error(err);
    throw std::runtime_error(err);
  }

  int bClamped = std::clamp(brightness, 0, (int)MAX_BRIGHTNESS);
  int sClamped = std::clamp(speed, 0, (int)MAX_SPEED);

  Logger::getInstance().info(
      "Aplicando modo LED " + std::to_string(mode) +
      " (R:" + std::to_string(r) + " G:" + std::to_string(g) +
      " B:" + std::to_string(b) + ", Brillo: " + std::to_string(bClamped) +
      ", Vel: " + std::to_string(sClamped) +
      ", Arcoíris: " + std::string(rainbow ? "Sí" : "No") + ")");

  ModePacket packet{};
  memset(&packet, 0, sizeof(packet));
  packet.mode = static_cast<uint8_t>(mode);
  packet.r = r;
  packet.g = g;
  packet.b = b;
  packet.options.rainbow = rainbow;
  packet.options.brightness = static_cast<uint8_t>(bClamped);
  packet.options.speed = static_cast<uint8_t>(sClamped);
  packet.options.direction = static_cast<uint8_t>(direction);
  packet.delimiter = 0xaa55;

  uint8_t data[PACKET_LENGTH];
  memcpy(data, &packet, PACKET_LENGTH);

  // Exact transactional sequence:
  this->sendPacket(START_PKT);
  this->sendPacket(START_MODE_PKT);
  this->handshake();
  this->sendPacket(data);
  this->sendPacket(SAVE_PKT);
  this->handshake();

  Logger::getInstance().success(
      "Modo de iluminación enviado y guardado exitosamente.");
}

void AK820Pro::setMode(const LightingMode mode, const uint8_t r,
                       const uint8_t g, const uint8_t b, const bool rainbow,
                       const int brightness, const int speed,
                       const Direction direction) const {
  std::lock_guard<std::recursive_mutex> lock(this->hidMutex);
  this->setModeUnlocked(mode, r, g, b, rainbow, brightness, speed, direction);
}

std::future<void> AK820Pro::setModeAsync(const LightingMode mode,
                                         const uint8_t r, const uint8_t g,
                                         const uint8_t b, const bool rainbow,
                                         const int brightness, const int speed,
                                         const Direction direction) const {
  auto promise = std::make_shared<std::promise<void>>();
  auto fut = promise->get_future();
  std::thread([this, mode, r, g, b, rainbow, brightness, speed, direction,
               promise]() {
    try {
      this->setMode(mode, r, g, b, rainbow, brightness, speed, direction);
      promise->set_value();
    } catch (const std::exception &e) {
      Logger::getInstance().error("Error en setModeAsync: " +
                                  std::string(e.what()));
      promise->set_exception(std::current_exception());
    }
  }).detach();
  return fut;
}

void AK820Pro::applyLighting(const LightingMode mode, const uint8_t r,
                             const uint8_t g, const uint8_t b,
                             const bool rainbow, const int brightness,
                             const int speed, const Direction direction,
                             const LightSleepTime sleep_delay) const {
  std::lock_guard<std::recursive_mutex> lock(this->hidMutex);
  this->setModeUnlocked(mode, r, g, b, rainbow, brightness, speed, direction);
  if (sleep_delay != NONE) {
    this->setSleepTimeUnlocked(sleep_delay);
  }
}

std::future<void> AK820Pro::applyLightingAsync(
    const LightingMode mode, const uint8_t r, const uint8_t g, const uint8_t b,
    const bool rainbow, const int brightness, const int speed,
    const Direction direction, const LightSleepTime sleep_delay) const {
  auto promise = std::make_shared<std::promise<void>>();
  auto fut = promise->get_future();
  std::thread([this, mode, r, g, b, rainbow, brightness, speed, direction,
               sleep_delay, promise]() {
    try {
      this->applyLighting(mode, r, g, b, rainbow, brightness, speed, direction,
                          sleep_delay);
      promise->set_value();
    } catch (const std::exception &e) {
      Logger::getInstance().error("Error en applyLightingAsync: " +
                                  std::string(e.what()));
      promise->set_exception(std::current_exception());
    }
  }).detach();
  return fut;
}

void AK820Pro::setPerKeyRGB(const std::vector<KeyColor> &keyColors) const {
  if (!this->handle && !const_cast<AK820Pro *>(this)->openHandle()) {
    std::string err =
        "Dispositivo HID no disponible: " + this->lastErrorMessage;
    Logger::getInstance().error(err);
    throw std::runtime_error(err);
  }

  Logger::getInstance().info(
      "Preparando datos de 144 teclas para LEDs individuales...");

  uint8_t buffer[576];
  memset(buffer, 0, sizeof(buffer));

  for (size_t i = 0; i < 144; i++) {
    buffer[i * 4] = static_cast<uint8_t>(i);
    buffer[i * 4 + 1] = 0;
    buffer[i * 4 + 2] = 0;
    buffer[i * 4 + 3] = 0;
  }

  for (const auto &kc : keyColors) {
    if (kc.keyIndex < 144) {
      buffer[kc.keyIndex * 4] = kc.keyIndex;
      buffer[kc.keyIndex * 4 + 1] = kc.r;
      buffer[kc.keyIndex * 4 + 2] = kc.g;
      buffer[kc.keyIndex * 4 + 3] = kc.b;
    }
  }

  uint8_t cmdCustom[PACKET_LENGTH] = {COMMAND_PREFIX,
                                      CUSTOM_LIGHT_COMMAND,
                                      0x00,
                                      0x00,
                                      0x00,
                                      0x00,
                                      0x00,
                                      0x00,
                                      0x09};

  this->sendPacket(START_PKT);
  this->sendPacket(cmdCustom);

  for (int p = 0; p < 9; p++) {
    uint8_t packet[PACKET_LENGTH] = {0};
    memcpy(packet, buffer + (p * 64), 64);
    this->sendPacket(packet);
  }

  this->sendPacket(SAVE_PKT);

  // Switch lighting mode to CUSTOM (0x80)
  this->setMode(CUSTOM, 255, 255, 255, false, MAX_BRIGHTNESS, MAX_SPEED, LEFT);
  Logger::getInstance().success("LEDs individuales aplicados con éxito.");
}

std::future<void>
AK820Pro::setPerKeyRGBAsync(const std::vector<KeyColor> &keyColors) const {
  auto promise = std::make_shared<std::promise<void>>();
  auto fut = promise->get_future();
  std::thread([this, keyColors, promise]() {
    try {
      this->setPerKeyRGB(keyColors);
      promise->set_value();
    } catch (const std::exception &e) {
      Logger::getInstance().error("Error en setPerKeyRGBAsync: " +
                                  std::string(e.what()));
      promise->set_exception(std::current_exception());
    }
  }).detach();
  return fut;
}

void AK820Pro::setSleepTimeUnlocked(const LightSleepTime sleep_time) const {
  if (!this->handle && !this->wirelessHandle &&
      !const_cast<AK820Pro *>(this)->openHandle()) {
    throw std::runtime_error("Dispositivo HID no disponible");
  }

  uint8_t data[PACKET_LENGTH] = {0};
  data[8] = static_cast<uint8_t>(sleep_time);
  data[62] = 0xaa;
  data[63] = 0x55;

  this->sendPacket(START_PKT);
  this->sendPacket(START_SLEEP_PKT);
  this->handshake();
  this->sendPacket(data);
  this->sendPacket(SAVE_PKT);
  this->handshake();
}

void AK820Pro::setSleepTime(const LightSleepTime sleep_time) const {
  std::lock_guard<std::recursive_mutex> lock(this->hidMutex);
  this->setSleepTimeUnlocked(sleep_time);
}

std::future<void>
AK820Pro::setSleepTimeAsync(const LightSleepTime sleep_time) const {
  auto promise = std::make_shared<std::promise<void>>();
  auto fut = promise->get_future();
  std::thread([this, sleep_time, promise]() {
    try {
      this->setSleepTime(sleep_time);
      promise->set_value();
    } catch (const std::exception &e) {
      Logger::getInstance().error("Error en setSleepTimeAsync: " +
                                  std::string(e.what()));
      promise->set_exception(std::current_exception());
    }
  }).detach();
  return fut;
}

void AK820Pro::syncClock(int slot) const {
  std::lock_guard<std::recursive_mutex> lock(this->hidMutex);
  if (!this->handle && !this->wirelessHandle &&
      !const_cast<AK820Pro *>(this)->openHandle()) {
    std::string err =
        "Dispositivo HID no disponible: " + this->lastErrorMessage;
    Logger::getInstance().error(err);
    throw std::runtime_error(err);
  }

  Logger::getInstance().info("Sincronizando reloj RTC con la pantalla...");
  auto now = std::chrono::system_clock::now();
  std::time_t tt = std::chrono::system_clock::to_time_t(now);
  std::tm local_tm{};
  localtime_r(&tt, &local_tm);

  uint8_t timeData[PACKET_LENGTH] = {0};
  timeData[0] = 0x00;
  timeData[1] = static_cast<uint8_t>(slot);
  timeData[2] = 0x5a;
  timeData[3] = static_cast<uint8_t>(local_tm.tm_year % 100);
  timeData[4] = static_cast<uint8_t>(local_tm.tm_mon + 1);
  timeData[5] = static_cast<uint8_t>(local_tm.tm_mday);
  timeData[6] = static_cast<uint8_t>(local_tm.tm_hour);
  timeData[7] = static_cast<uint8_t>(local_tm.tm_min);
  timeData[8] = static_cast<uint8_t>(local_tm.tm_sec);
  timeData[9] = 0x00;
  timeData[10] = 0x04;
  timeData[62] = 0xaa;
  timeData[63] = 0x55;

  this->sendPacket(START_PKT);
  this->sendPacket(START_TIME_PKT);
  this->handshake();
  this->sendPacket(timeData);
  this->sendPacket(SAVE_PKT);
  this->handshake();

  Logger::getInstance().success("Reloj RTC sincronizado con éxito.");
}

std::future<void> AK820Pro::syncClockAsync(int slot) const {
  auto promise = std::make_shared<std::promise<void>>();
  auto fut = promise->get_future();
  std::thread([this, slot, promise]() {
    try {
      this->syncClock(slot);
      promise->set_value();
    } catch (const std::exception &e) {
      Logger::getInstance().error("Error en syncClockAsync: " +
                                  std::string(e.what()));
      promise->set_exception(std::current_exception());
    }
  }).detach();
  return fut;
}

void AK820Pro::uploadImageToLCD(const std::string &path, int slot) const {
  std::lock_guard<std::recursive_mutex> lock(this->hidMutex);

  // The AK820 Pro hardware only features 1 user animation slot (Slot 1)
  int targetSlot = 1;
  if (slot != 1) {
    Logger::getInstance().warning(
        "El AK820 Pro dispone de 1 ranura en hardware para imagen personalizada. Redirigiendo a ranura 1.");
  }

  if (!std::filesystem::exists(path)) {
    std::string err = "El archivo no existe: " + path;
    Logger::getInstance().error(err);
    throw std::runtime_error(err);
  }

  if (!this->handle && !const_cast<AK820Pro *>(this)->openHandle()) {
    std::string err =
        "Dispositivo HID no disponible: " + this->lastErrorMessage;
    Logger::getInstance().error(err);
    throw std::runtime_error(err);
  }

  if (this->dispFd < 0) {
    std::string err =
        this->wirelessMode
            ? "La pantalla TFT solo puede actualizarse con el cable USB "
              "conectado (el modo 2.4G no soporta transferencia de pantalla)."
            : "Canal de pantalla TFT (/dev/hidraw con usage_page 0xFF68) no "
              "disponible. Comprueba permisos udev.";
    Logger::getInstance().error(err);
    throw std::runtime_error(err);
  }

  Logger::getInstance().info("Procesando imagen/animación: " + path);
  std::vector<uint8_t> payload = processImageOrGif(path);
  uint8_t nFrames = payload[0];
  Logger::getInstance().info(
      "Fotogramas preparados: " + std::to_string(nFrames) +
      " (Total datos: " + std::to_string(payload.size()) + " bytes)");

  uint16_t totalChunks = static_cast<uint16_t>(
      (payload.size() + IMAGE_CHUNK_SIZE - 1) / IMAGE_CHUNK_SIZE);
  this->totalChunksCount = totalChunks;
  this->currentChunk = 0;
  this->uploadProgress = 0.0f;
  this->uploading = true;
  this->uploadStatusDetail = "Transfiriendo datos a la pantalla...";

  Logger::getInstance().info("Iniciando transferencia TFT en ranura " +
                             std::to_string(targetSlot) + " (" +
                             std::to_string(totalChunks) + " bloques)...");

  uint8_t cmdImage[PACKET_LENGTH] = {0};
  cmdImage[0] = COMMAND_PREFIX;
  cmdImage[1] = IMAGE_COMMAND;
  cmdImage[2] = static_cast<uint8_t>(targetSlot);
  cmdImage[8] = static_cast<uint8_t>(totalChunks & 0xFF);
  cmdImage[9] = static_cast<uint8_t>((totalChunks >> 8) & 0xFF);

  // Synchronize transaction with keyboard MCU
  this->sendPacket(START_PKT);
  this->handshake();
  this->sendPacket(cmdImage);
  this->handshake();

  uint8_t chunk_buf[1 + IMAGE_CHUNK_SIZE];
  chunk_buf[0] = 0x00; // report ID 0

  for (uint16_t i = 0; i < totalChunks; i++) {
    size_t offset = i * IMAGE_CHUNK_SIZE;
    size_t len = std::min<size_t>(IMAGE_CHUNK_SIZE, payload.size() - offset);
    memcpy(chunk_buf + 1, payload.data() + offset, len);
    if (len < IMAGE_CHUNK_SIZE) {
      memset(chunk_buf + 1 + len, 0, IMAGE_CHUNK_SIZE - len);
    }

    ssize_t written = ::write(this->dispFd, chunk_buf, sizeof(chunk_buf));
    if (written < 0) {
      this->uploading = false;
      std::string err = "Error escribiendo bloque " + std::to_string(i + 1) +
                        " en el canal de pantalla.";
      this->uploadStatusDetail = err;
      Logger::getInstance().error(err);
      throw std::runtime_error(err);
    }

    struct pollfd pfd = {this->dispFd, POLLIN, 0};
    int pRet = ::poll(&pfd, 1, 300);
    if (pRet > 0) {
      uint8_t ackBuf[64];
      ssize_t rBytes = ::read(this->dispFd, ackBuf, sizeof(ackBuf));
      (void)rBytes;
    }

    this->currentChunk = i + 1;
    this->uploadProgress = static_cast<float>(i + 1) / totalChunks;

    if ((i + 1) % 4 == 0 || i + 1 == totalChunks) {
      Logger::getInstance().info(
          "Bloque " + std::to_string(i + 1) + "/" +
          std::to_string(totalChunks) + " transferido (" +
          std::to_string(static_cast<int>(this->uploadProgress * 100)) + "%).");
    }
  }

  // Finalize and commit transaction to flash memory
  this->sendPacket(SAVE_PKT);
  this->handshake();

  this->uploadProgress = 1.0f;
  this->uploading = false;
  this->uploadStatusDetail =
      "Imagen cargada exitosamente. Presiona FN + Del en el teclado para verla.";

  try {
    const char *home = secure_getenv("HOME");
    if (home) {
      std::string galDir = std::string(home) + "/.config/ak820proLED/gallery";
      std::filesystem::create_directories(galDir);
      std::filesystem::path srcP(path);
      std::string destP = galDir + "/slot" + std::to_string(targetSlot) + "_" +
                          srcP.filename().string();
      std::filesystem::copy_file(
          srcP, destP, std::filesystem::copy_options::overwrite_existing);
    }
  } catch (...) {
  }

  Logger::getInstance().success(this->uploadStatusDetail);
}

std::future<void> AK820Pro::uploadImageToLCDAsync(const std::string &path,
                                                  int slot) const {
  auto promise = std::make_shared<std::promise<void>>();
  auto fut = promise->get_future();
  std::thread([this, path, slot, promise]() {
    try {
      this->uploadImageToLCD(path, slot);
      promise->set_value();
    } catch (const std::exception &e) {
      Logger::getInstance().error("Error en uploadImageToLCDAsync: " +
                                  std::string(e.what()));
      promise->set_exception(std::current_exception());
    }
  }).detach();
  return fut;
}

void AK820Pro::uploadGIF(const std::string &path) const {
  this->uploadImageToLCD(path, 1);
}
