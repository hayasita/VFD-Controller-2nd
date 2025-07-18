#pragma once

#include <M5UnitOLED.h>
#include <M5Unified.h>
#include "I2CBusManager.h"

/**
 * @brief M5UnitOLEDの管理クラス
 */
class M5oledManager {
public:
  M5oledManager(I2CBusManager* busManager);

  void begin(void); // M5UnitOLEDの初期化
  void showMessage(uint8_t positionX = 0, uint8_t positionY = 0, const char* message = nullptr);  // M5UnitOLEDにメッセージを表示

private:
  I2CBusManager* i2cBus;
  M5UnitOLED oled;
};


/**
 * @brief ディスプレイ制御クラス
 */
class DisplayManager {
public:
  DisplayManager(I2CBusManager* busManager);
  ~DisplayManager() = default;
  void begin(void);

  void showMessage(const char* message);
  void showTime(int hour, int minute, int second, int position = 0, const char* header = "");

private:
  I2CBusManager* i2cBus = nullptr;
  M5oledManager m5oledManager;  // M5UnitOLEDの管理クラス
};
