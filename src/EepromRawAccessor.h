#pragma once

#include <mutex>
#include "I2CBusManager.h"
#include <EEPROM.h>

// EEPROMの低レベル読み書き管理
class EepromRawAccessor {
public:
  // EEPROMの初期化
  void begin(I2CBusManager& busManager);

  // バイト単位でデータを書き込む
  bool writeBytes(int address, const void* data, size_t len);

  // バイト単位でデータを読み込む
  bool readBytes(int address, void* data, size_t len);

  bool sequentialRead(uint8_t i2cADR, uint16_t eeADR ,uint8_t *data,uint8_t dataNum);

private:
  I2CBusManager* i2cBus = nullptr;  // I2Cバスマネージャ

  static constexpr size_t EEPROM_SIZE = 512; // EEPROMのサイズ
  uint8_t* eeprom = nullptr;  // EEPROMデータポインタ
};
