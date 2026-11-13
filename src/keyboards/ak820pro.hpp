//
// Created by taxmachine on 16/02/25.
// Modified by Vixcho on 13/10/26.
//

#ifndef AJAZZ_HPP
#define AJAZZ_HPP

#include <array>
#include <atomic>
#include <cstdint>
#include <future>
#include <hidapi/hidapi.h>
#include <iomanip>
#include <mutex>
#include <string>
#include <vector>

#include "keyboarddefs.hpp"

class AK820Pro {
public:
  struct __attribute__((packed)) ModePacket {
    uint8_t mode;        // 0
    uint8_t r;           // 1
    uint8_t g;           // 2
    uint8_t b;           // 3
    uint32_t padding;    // 4..7
    struct {
      bool rainbow;      // 8
      uint8_t brightness;// 9
      uint8_t speed;     // 10
      uint8_t direction; // 11
    } options;
    uint16_t padding1;   // 12..13
    uint16_t delimiter;  // 14..15 (0xaa55)
    uint8_t additional_data[48]; // 16..63
  };

  struct KeyColor {
    uint8_t keyIndex;
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  enum LightingMode {
    LED_OFF = 0,
    STATIC = 1,
    SINGLE_ON = 2,
    SINGLE_OFF = 3,
    GLITTERING = 4,
    FALLING = 5,
    COLOURFUL = 6,
    BREATH = 7,
    SPECTRUM = 8,
    OUTWARD = 9,
    SCROLLING = 10,
    ROLLING = 11,
    ROTATING = 12,
    EXPLODE = 13,
    LAUNCH = 14,
    RIPPLES = 15,
    FLOWING = 16,
    PULSATING = 17,
    TILT = 18,
    SHUTTLE = 19,
    CUSTOM = 0x80
  };

  enum Direction { LEFT = 0, DOWN = 1, UP = 2, RIGHT = 3 };

  enum LightSleepTime { NONE = 0, ONE_MIN = 1, FIVE_MIN = 2, THIRTY_MIN = 3 };

  explicit AK820Pro(uint16_t vid = AJAZZ_VID, uint16_t pid = 0);
  ~AK820Pro();

  bool openHandle();
  void closeHandle();

  bool isConnected() const;
  bool isWireless() const;
  bool isDisplayAvailable() const;
  uint16_t getPid() const { return this->pid; }
  std::string getConnectionStatus() const;
  std::string getLastError() const;

  bool isUploading() const { return this->uploading.load(); }
  float getUploadProgress() const { return this->uploadProgress.load(); }
  int getUploadChunk() const { return this->currentChunk.load(); }
  int getTotalChunks() const { return this->totalChunksCount.load(); }
  std::string getUploadStatusDetail() const { return this->uploadStatusDetail; }

  void setColor(uint8_t r, uint8_t g, uint8_t b) const;
  void setMode(LightingMode mode, uint8_t r = 255, uint8_t g = 0, uint8_t b = 0,
               bool rainbow = true, int brightness = MAX_BRIGHTNESS,
               int speed = MAX_SPEED, Direction direction = LEFT) const;
  std::future<void> setModeAsync(LightingMode mode, uint8_t r, uint8_t g,
                                 uint8_t b, bool rainbow, int brightness,
                                 int speed, Direction direction) const;

  void applyLighting(LightingMode mode, uint8_t r, uint8_t g, uint8_t b,
                     bool rainbow, int brightness, int speed,
                     Direction direction, LightSleepTime sleep_delay) const;
  std::future<void> applyLightingAsync(LightingMode mode, uint8_t r, uint8_t g,
                                       uint8_t b, bool rainbow, int brightness,
                                       int speed, Direction direction,
                                       LightSleepTime sleep_delay) const;

  void setPerKeyRGB(const std::vector<KeyColor> &keyColors) const;
  std::future<void>
  setPerKeyRGBAsync(const std::vector<KeyColor> &keyColors) const;

  void setSleepTime(LightSleepTime sleep_time) const;
  std::future<void> setSleepTimeAsync(LightSleepTime sleep_time) const;

  void syncClock(int slot = 1) const;
  std::future<void> syncClockAsync(int slot = 1) const;

  void uploadGIF(const std::string &path) const;
  void uploadImageToLCD(const std::string &path, int slot = 1) const;
  std::future<void> uploadImageToLCDAsync(const std::string &path,
                                          int slot = 1) const;

private:
  uint16_t vid;
  uint16_t pid;
  mutable hid_device *handle = nullptr;
  mutable hid_device *wirelessHandle = nullptr;
  mutable std::recursive_mutex hidMutex;
  mutable int dispFd = -1;
  mutable std::string activePath;
  mutable std::string dispPath;
  mutable bool wirelessMode = false;
  mutable std::string lastErrorMessage;

  mutable std::atomic<bool> uploading{false};
  mutable std::atomic<float> uploadProgress{0.0f};
  mutable std::atomic<int> currentChunk{0};
  mutable std::atomic<int> totalChunksCount{0};
  mutable std::string uploadStatusDetail;

  bool sendPacket(const uint8_t *payload64) const;
  void handshake() const;
  void setModeUnlocked(LightingMode mode, uint8_t r, uint8_t g, uint8_t b,
                       bool rainbow, int brightness, int speed,
                       Direction direction) const;
  void setSleepTimeUnlocked(LightSleepTime sleep_time) const;
};

static_assert(sizeof(AK820Pro::ModePacket) == 64,
              "ModePacket must be exactly 64 bytes");

#endif // AJAZZ_HPP