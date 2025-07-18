#include "DisplayManager.h"
#include "Config.h"

DisplayManager::DisplayManager(I2CBusManager* busManager)
  : i2cBus(busManager), m5oledManager(busManager)  // M5UnitOLEDの初期化
{
  return;
}

void DisplayManager::begin(void) {

  if(i2cBus->isM5oledConnected()){
    m5oledManager.begin();  // M5UnitOLEDの初期化
  }

  return;
}

void DisplayManager::showMessage(const char* message) {

  if(i2cBus->isM5oledConnected()){
    m5oledManager.showMessage(0, 10, message);  // M5UnitOLEDにメッセージを表示
  }

  return;
}

void DisplayManager::showTime(int hour, int minute, int second, int position, const char* header) {
  char timeStr[16];
  snprintf(timeStr, sizeof(timeStr), "%s:%02d:%02d:%02d", header, hour, minute, second);

  if(i2cBus->isM5oledConnected()){
    m5oledManager.showMessage(0, position, timeStr);
  }

  return;
}

/**
 * @brief M5UnitOLEDの管理クラスのコンストラクタ
 */
M5oledManager::M5oledManager(I2CBusManager* busManager)
  : i2cBus(busManager), oled(M5UnitOLED())
{
  // M5UnitOLEDの初期化
}

/**
 * @brief M5UnitOLEDの初期化
 */
void M5oledManager::begin(void) {
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());
  
  oled.init(SDA_PIN, SCL_PIN, I2C_FREQ);  // SDA,SCL必須
  oled.setRotation(1);            // テキストの表示方向を縦方向に設定
  oled.setTextSize(1);
  oled.setCursor(0, 0);           // テキストのカーソル位置を左上に設定。
  oled.startWrite();
  oled.print("hello OLED!!");
  oled.endWrite();
}

/**
 * @brief M5UnitOLEDにメッセージを表示
 * @param positionX X座標 (0-63)
 * @param positionY Y座標 (0-31)
 * @param message メッセージ
 */
void M5oledManager::showMessage(uint8_t positionX, uint8_t positionY, const char* message) {
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());

  oled.setCursor(positionX, positionY);
  oled.print(message);
  oled.endWrite();
}