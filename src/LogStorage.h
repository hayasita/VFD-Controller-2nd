#pragma once

#include "EepromManager.h"

// ログのストレージ操作クラス
class LogStorage {
public:
  // EepromManagerとの紐付け
  void begin(EepromManager& eeprom);

  // ログメッセージを書き込む
  bool write(int address, const char* message, size_t len);

  // ログメッセージを読み込む
  bool read(int address, char* buffer, size_t len);

private:
  EepromManager* eeprom = nullptr;  // EepromManagerの参照
};
