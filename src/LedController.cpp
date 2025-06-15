#include "LedController.h"

LedController::LedController(CRGB* leds, int numLeds)
  : leds_(leds), numLeds_(numLeds) {
  states_ = new LedState[numLeds_];
}

void LedController::setMode(int idx, LedMode mode, CRGB color) {
  if(idx < 0 || idx >= numLeds_) return;
  states_[idx].mode = mode;
  states_[idx].color = color;
}

void LedController::update() {
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
        if(now - states_[i].lastToggle > 500) {
          states_[i].state = !states_[i].state;
          states_[i].lastToggle = now;
        }
        leds_[i] = states_[i].state ? states_[i].color : CRGB::Black;
        break;
      }
  }
  FastLED.show();
}

#ifdef DELETE
void LedController::handleBlink() {
    uint32_t now = millis();
    if(now - lastUpdate_ > 500) {
        state_ = !state_;
        fill_solid(leds_.data(), numLeds_, state_ ? color_ : CRGB::Black);
        show();
        lastUpdate_ = now;
    }
}

void LedController::handleRainbow() {
    static uint8_t hue = 0;
    fill_rainbow(leds_.data(), numLeds_, hue++);
    show();
}

void LedController::show() {
    FastLED.show();
}

void LedController::setCustomPattern(void (*patternFunc)(CRGB*, int, uint32_t)) {
    customPattern_ = patternFunc;
    mode_ = Mode::Custom;
}
#endif