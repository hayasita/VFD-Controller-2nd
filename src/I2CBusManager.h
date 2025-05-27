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

/**
 * @brief I2Cバスアクセス排他管理クラス
 */
class I2CBusManager {
public:
  virtual void begin();                     // I2Cバスの初期化
  virtual std::recursive_mutex& getMutex(); // I2Cバスのミューテックスを取得
  virtual TwoWire& getWire();               // I2CバスのWireオブジェクトを取得

  virtual std::vector<uint8_t> scanI2CBus(uint8_t address = 0x00, uint8_t count = 127); // I2Cバスのスキャン

private:
  std::recursive_mutex i2cMutex;
  TwoWire wire = TwoWire(0);  // 0番ポートを使用（ESP32）
};
