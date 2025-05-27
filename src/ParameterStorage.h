#pragma once

#include "EepromManager.h"

// パラメータのストレージ操作クラス
class ParameterStorage {
public:
  // EepromManagerとの紐付け
  void begin(EepromManager& eeprom);

  // パラメータの保存
  void save(uint8_t index, int value);

  // パラメータの読み込み
  bool load(uint8_t index, int& value);

private:
  EepromManager* eeprom = nullptr;  // EepromManagerの参照
  static constexpr int PARAM_START_ADDR = 0;  // パラメータの開始アドレス
};
