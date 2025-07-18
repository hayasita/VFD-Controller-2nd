#pragma once

#ifdef UNIT_TEST
// ...モック定義...
#include "../test/mock/Wire.h"
#else
// ...本来の定義...
#include <Wire.h>
#endif

#include <mutex>
#include <vector>
#include <map>
#include <string>

#define I2CADR_M5OLED   0x3c  // M5 OLEDディスプレイのI2Cアドレス
#define I2CADR_SSD1306  0x3d  // SSD1306 OLEDディスプレイのI2Cアドレス
#define I2CADR_EEPROM   0x50  // EEPROMのI2Cアドレス
/**
 * I2Cデバイス名マップ
 */
const std::map<uint8_t, std::string> i2cDeviceNameMap = {
  {I2CADR_M5OLED,   "M5 OLED Display SH1107"},
  {I2CADR_SSD1306,  "SSD1306 OLED Display"},
  {I2CADR_EEPROM,   "EEPROM 24C32"},
  {0x51, "M5 RTC"},
  {0x68, "RTC"},
  {0x70, "QMP6988"},
  {0x77, "BME680"}
};

/**
 * @brief I2Cバスアクセス排他管理クラス
 */
class I2CBusManager {
public:
  virtual void begin();                     // I2Cバスの初期化
  virtual std::recursive_mutex& getMutex(); // I2Cバスのミューテックスを取得
  virtual TwoWire& getWire();               // I2CバスのWireオブジェクトを取得

  virtual std::vector<uint8_t> scanI2CBus(uint8_t address = 0x00, uint8_t count = 127); // I2Cバスのスキャン
  virtual std::string getDeviceName(uint8_t address); // デバイス名を取得

  virtual bool isEepromConnected();         // EEPROMが接続されているか確認
  virtual bool isM5oledConnected();         // M5 OLEDが接続されているか確認
private:
  std::recursive_mutex i2cMutex;
//  TwoWire wire = TwoWire(0);  // 0番ポートを使用（ESP32）
  std::vector<uint8_t> i2cDevice; // スキャン結果のアドレスリスト

};
#define wire Wire
