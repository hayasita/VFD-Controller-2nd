#include "LedController.h"

LedController::LedController(CRGB* leds, int numLeds)
  : leds_(leds), numLeds_(numLeds)
{
  if(numLeds_ <= 0 || leds_ == nullptr) {
    numLeds_ = 0;
    leds_ = nullptr;
    return;
  }

  LedState defaultState;                  // デフォルトのLED状態
  defaultState.mode = LedMode::Off;       // 初期状態は全てオフ
  defaultState.color = CRGB::Black;       // 初期色は黒
  defaultState.lastToggle = 0;            // 最後のトグル時間は0
  defaultState.state = false;             // 初期状態は消灯
  states_.resize(numLeds_, defaultState); // 初期状態を設定
}

void LedController::setMode(int idx, LedMode mode, CRGB color) {
  if(idx < 0 || idx >= numLeds_) return;
  states_[idx].mode = mode;
  states_[idx].color = color;
}

void LedController::update(void)
{
  uint32_t now = millis();
  for(int i = 0; i < numLeds_; ++i) {
    switch(states_[i].mode) {
      case LedMode::Off:
        leds_[i] = CRGB::Black;
        break;
      case LedMode::On:
        leds_[i] = states_[i].color;
        break;
      case LedMode::Blink:
        handleBlink(leds_[i], states_[i]);
        break;
    }
  }
  show();

  return;
}

void LedController::show(void) {
  FastLED.show();
}

void LedController::handleBlink(CRGB& led, LedState& state) {
  uint32_t now = millis();
  if(now - state.lastToggle > 500) {
    state.state = !state.state;
    state.lastToggle = now;
  }
  led = state.state ? state.color : CRGB::Black;

  return;
}
