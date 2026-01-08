#pragma once
#include <cstdint>
#include <vector>
#include <FastLED.h>
#include "LedColor.h"

enum class LedMode { Off, On, Blink };

struct LedState {
  LedMode mode = LedMode::Off;  // LEDのモード
  CRGB color = CRGB::Black;     // LEDの色
  uint32_t lastToggle = 0;      // 最後のトグル時間
  bool state = false;           // 状態（点灯/消灯）管理
};

class LedController {
public:
  LedController(CRGB* leds = nullptr, int numLeds = 0);
  void setMode(int idx, LedMode mode, Colorld color = Colorld::White);
  void update(void);

private:
  int numLeds_;                   // LEDの数
  CRGB* leds_;                    // LEDの配列
  std::vector<LedState> states_;  // LEDの状態を管理する配列

  void show(void);
  void handleBlink(CRGB& led, LedState& state_);
};
