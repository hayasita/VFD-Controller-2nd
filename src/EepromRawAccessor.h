#pragma once

#include <mutex>
#include "I2CBusManager.h"
#include "Config.h"
#include <EEPROM.h>

// EEPROMの低レベル読み書き管理
class EepromRawAccessor {
public:
  // EEPROMの初期化
  EepromRawAccessor(I2CBusManager *busManager);  // デフォルトコンストラクタ

  void begin(void);

//  bool readBytes(int address, void* data, size_t len);          // バイト単位でデータを読み込む
//  bool writeBytes(int address, const void* data, size_t len);   // バイト単位でデータを書き込む

  bool writeByte(uint16_t address, uint8_t data);               // データを1バイト書き込む
  bool readByte(uint16_t address, uint8_t *data);               // データを1バイト読み込む
  bool sequentialRead(uint16_t eeADR ,uint8_t *data,uint8_t dataNum);

private:
  I2CBusManager* i2cBus = nullptr;  // I2Cバスマネージャ

//  static constexpr size_t EEPROM_SIZE = 512; // EEPROMのサイズ
  static constexpr size_t EEPROM_SIZE = EEPROM_MAX_ADDRESS; // EEPROMのサイズ
  uint8_t* eeprom = nullptr;  // EEPROMデータポインタ

  bool i2cReadByte(uint16_t address, uint8_t *data);               // データを1バイト読み込む
  bool i2cWriteByte(uint16_t address, uint8_t data);               // データを1バイト書き込む
  bool i2cSequentialRead(uint8_t i2cADR, uint16_t eeADR ,uint8_t *data,uint8_t dataNum);
};
