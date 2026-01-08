#pragma once

#include "LedColor.h"

#ifndef UNIT_TEST
#include "LedController.h"
#else
enum class LedMode { Off, On, Blink };
class LedController{
public:
  LedController(uint8_t*, int) {}  
  void setMode(int, LedMode, Colorld) {}
}; // LedControllerの前方宣言（実際のLedControllerクラスは別ファイルに定義されている）
#endif

#define BUILTIN_LED_NUM 1               // 内蔵LEDの数
#define EXTERNAL_LED_NUM 3              // 外部LEDの数
#define BUILTIN_LED_DATA_PIN 21         // 内蔵RGBLEDのDATA PIN
#define EXTERNAL_LED_DATA_PIN 14        // 外部RGBLEDのDATA PIN
#define LED_MAX_BRIGHTNESS 16           // LEDの明るさ（20以上は熱で壊れる可能性あり。）

class LedManager {
public:
  LedManager(void);
  LedController builtInLedCtrl;
  LedController externalLedCtrl;
    
  void reset(void);
private:

};
