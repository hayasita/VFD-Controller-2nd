#pragma once
#include <FastLED.h>

#define NUM_BUILTIN_LEDS 1
#define NUM_EXTERNAL_LEDS 3
#define BUILTIN_LED_DATA_PIN 21     // 内蔵RGBLEDのDATA PIN
#define EXTERNAL_LED_DATA_PIN 14    // 外部RGBLEDのDATA PIN
#define LED_MAX_BRIGHTNESS 16       // LEDの明るさ（20以上は熱で壊れる可能性あり。）

enum class LedMode { Off, On, Blink };

struct LedState {
    LedMode mode = LedMode::Off;
    CRGB color = CRGB::Black;
    uint32_t lastToggle = 0;
    bool state = false;
};

class LedController {
public:
    LedController(CRGB* leds, int numLeds);
    void setMode(int idx, LedMode mode, CRGB color = CRGB::White);
    void update();

private:
    CRGB* leds_;
    int numLeds_;
    LedState* states_;

    void show();
    void handleBlink();
    void handleRainbow();
};
