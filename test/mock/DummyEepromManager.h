#ifndef DUMMY_EEPROM_MANAGER_H
#define DUMMY_EEPROM_MANAGER_H

#include "../../src/EepromManager.h"  // 実際の EepromManager クラスのヘッダーファイルをインクルード

class DummyEepromManager : public EepromManager {
public:
  void begin(I2CBusManager& busManager) override {
    // モックの初期化処理
    memset(memory, 0, SIZE);  // メモリを初期化
  }

  std::recursive_mutex& getMutex() override {
    return dummyMutex;
  }

  bool writeBytes(int address, const void* data, size_t len) override {
    // モックの EEPROM 書き込み処理
    memcpy(&memory[address], data, len);  // memcpy を使用
    return true;
  }

  bool readBytes(int address, void* data, size_t len) override {
    // モックの EEPROM 読み込み処理
    memcpy(data, &memory[address], len);  // memcpy を使用
    return true;
  }

  bool readMultipleBytes(int address, uint8_t *data, size_t len) override {
    // モックの EEPROM 複数バイト読み込み処理
    memcpy(data, &memory[address], len);  // memcpy を使用
    return true;
  }

private:
  static const int SIZE = 1024;
  uint8_t memory[SIZE];
  std::recursive_mutex dummyMutex;
};

#endif // DUMMY_EEPROM_MANAGER_H
