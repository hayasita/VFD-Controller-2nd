#pragma once

#include <M5UnitOLED.h>
#include <M5Unified.h>
//#include <Wire.h>
#include <RTClib.h>
#include "I2CBusManager.h"

/**
 * @brief OLEDディスプレイ制御クラス
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
  M5UnitOLED oled;
};
