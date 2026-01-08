#include "LedManager.h"

#ifndef UNIT_TEST
CRGB builtInLeds[BUILTIN_LED_NUM];
CRGB externalLeds[EXTERNAL_LED_NUM];
#else
uint8_t *builtInLeds;
uint8_t *externalLeds;
#endif

LedManager::LedManager(void)
  : builtInLedCtrl(builtInLeds, BUILTIN_LED_NUM),
    externalLedCtrl(externalLeds, EXTERNAL_LED_NUM)
{
#ifndef UNIT_TEST
  FastLED.addLeds<WS2812B, BUILTIN_LED_DATA_PIN, GRB>(builtInLeds, BUILTIN_LED_NUM).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, EXTERNAL_LED_DATA_PIN, GRB>(externalLeds, EXTERNAL_LED_NUM).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED_MAX_BRIGHTNESS);
#endif
  return;
}

void LedManager::reset(void){
  builtInLedCtrl.setMode(0, LedMode::Off, Colorld::Red);
  externalLedCtrl.setMode(0, LedMode::On, Colorld::Green);
  externalLedCtrl.setMode(1, LedMode::Blink, Colorld::Blue);
  externalLedCtrl.setMode(2, LedMode::On, Colorld::Orange);
};
