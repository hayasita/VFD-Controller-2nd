/**
 * @file ParameterStorage.h
 * @author
 * @brief パラメータのストレージ操作クラス
 * @version 0.1
 * @date 2025-06-10
 * 
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "EepromManager.h"

class ParameterStorage {
public:
  ParameterStorage(EepromManager *eeprom);
  ~ParameterStorage();

  // パラメータの保存
  void save(uint8_t index, int value);

  // パラメータの読み込み
  bool load(uint8_t index, uint8_t *value);

private:
  EepromManager* eeprom = nullptr;  // EepromManagerの参照
  static constexpr int PARAM_START_ADDR = 0x0010;  // パラメータの開始アドレス
};
