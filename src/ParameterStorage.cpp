/**
 * @file ParameterStorage.cpp
 * @author
 * @brief パラメータのストレージ操作クラス
 * @version 0.1
 * @date 2025-06-10
 * 
 * @copyright Copyright (c) 2025
 */

#include "ParameterStorage.h"

/**
 * @brief Construct a new Parameter Storage:: Parameter Storage object
 * 
 * @param eeprom EepromManagerのポインタ
 */
ParameterStorage::ParameterStorage(EepromManager *eeprom)
  : eeprom(eeprom) {  // EepromManagerの参照を初期化
}

/**
 * @brief Destroy the Parameter Storage:: Parameter Storage object
 * 
 * EepromManagerの参照を解放します。
 */ 
ParameterStorage::~ParameterStorage(){
  eeprom = nullptr;  // EepromManagerの参照を解放
}

/**
 * @brief パラメータの保存
 * @param index パラメータのインデックス
 * @param value 保存する値
 */
void ParameterStorage::save(uint8_t index, int value) {
  eeprom->writeByte(PARAM_START_ADDR + index, value);  // パラメータ保存
}

/**
 * @brief パラメータの読み込み
 * @param index パラメータのインデックス
 * @param value 読み込んだ値を格納するポインタ
 * @return true 成功、false 失敗
 */
bool ParameterStorage::load(uint8_t index, uint8_t *value) {
  return eeprom->readByte(PARAM_START_ADDR + index, value);  // パラメータ読み込み
}
