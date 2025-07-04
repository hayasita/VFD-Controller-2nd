#include "DisplayManager.h"
#include "Config.h"

DisplayManager::DisplayManager(I2CBusManager* busManager)
  : i2cBus(busManager), oled(M5UnitOLED())  // M5UnitOLEDの初期化
{
  return;
}

void DisplayManager::begin(void) {
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());

  oled.init(SDA_PIN, SCL_PIN, I2C_FREQ);  // SDA,SCL必須
  oled.setRotation(1);            // テキストの表示方向を縦方向に設定
  oled.setTextSize(1);
  oled.setCursor(0, 0);           // テキストのカーソル位置を左上に設定。
  oled.startWrite();
  oled.print("hello OLED!!");
  oled.endWrite();
}

void DisplayManager::showMessage(const char* message) {

  oled.setCursor(0, 10);
  oled.print(message);
  oled.endWrite();

}

void DisplayManager::showTime(int hour, int minute, int second, int position, const char* header) {
  char timeStr[16];
  snprintf(timeStr, sizeof(timeStr), "%s:%02d:%02d:%02d", header, hour, minute, second);

  oled.setCursor(0, position);
  oled.print(timeStr);
  oled.endWrite();

}

