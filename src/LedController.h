#pragma once
#include <cstdint>
#include <vector>
#include <FastLED.h>

#define NUM_BUILTIN_LEDS 1            // 内蔵LEDの数
#define NUM_EXTERNAL_LEDS 3 //8       // 外部LEDの数
#define BUILTIN_LED_DATA_PIN 21       // 内蔵RGBLEDのDATA PIN
#define EXTERNAL_LED_DATA_PIN 14//43  // 外部RGBLEDのDATA PIN
#define LED_MAX_BRIGHTNESS 16         // LEDの明るさ（20以上は熱で壊れる可能性あり。）

enum class LedMode { Off, On, Blink };

struct LedState {
  LedMode mode = LedMode::Off;  // LEDのモード
  CRGB color = CRGB::Black;     // LEDの色
  uint32_t lastToggle = 0;      // 最後のトグル時間
  bool state = false;           // 状態（点灯/消灯）管理
};

class LedController {
public:
  LedController(CRGB* leds, int numLeds);
  void setMode(int idx, LedMode mode, CRGB color = CRGB::White);
  void update(void);

private:
  int numLeds_;                   // LEDの数
  CRGB* leds_;                    // LEDの配列
  std::vector<LedState> states_;  // LEDの状態を管理する配列

  void show(void);
  void handleBlink(CRGB& led, LedState& state_);
};
