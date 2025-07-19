#pragma once

#include <mutex>
#include <string>
#include "I2CBusManager.h"
#include "EepromRawAccessor.h"

// EEPROMアクセス排他制御管理
class EepromManager {
public:
  // 初期化
  EepromManager(I2CBusManager *busManager);  // デフォルトコンストラクタ

  virtual void begin(void);

  // EEPROMアクセス用のミューテックスを取得
  virtual std::recursive_mutex& getMutex();

  // バイト単位でデータを書き込む
//  virtual bool writeBytes(int address, const void* data, size_t len);
  virtual bool writeByte(uint16_t address, const uint8_t data);

  // バイト単位でデータを読み込む
//  virtual bool readBytes(int address, void* data, size_t len);
  virtual bool readByte(uint16_t address, uint8_t *data);

  virtual bool readMultipleBytes(int address, uint8_t *data, size_t len);   // i2c EEPROMから複数バイト読み込み

  std::string dumpEepromData(uint16_t address = 0x00, uint16_t len = 16);

private:
  I2CBusManager* i2cBus = nullptr;  // I2Cバスマネージャ
  std::recursive_mutex eepromMutex;  // EEPROM用のミューテックス
  EepromRawAccessor rawAccessor;     // 実際のEEPROMデータ操作クラス

};
