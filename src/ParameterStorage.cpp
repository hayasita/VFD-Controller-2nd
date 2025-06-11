#include "ParameterStorage.h"

ParameterStorage::ParameterStorage(EepromManager *eeprom)
  : eeprom(eeprom) {  // EepromManagerの参照を初期化
}
ParameterStorage::~ParameterStorage(){
  eeprom = nullptr;  // EepromManagerの参照を解放
}

void ParameterStorage::save(uint8_t index, int value) {
//  eeprom->writeBytes(PARAM_START_ADDR + index * sizeof(int), &value, sizeof(int));  // パラメータ保存
  eeprom->writeByte(PARAM_START_ADDR + index, value);  // パラメータ保存
}

bool ParameterStorage::load(uint8_t index, uint8_t *value) {
//  return eeprom->readBytes(PARAM_START_ADDR + index * sizeof(int), &value, sizeof(int));  // パラメータ読み込み
  return eeprom->readByte(PARAM_START_ADDR + index, value);  // パラメータ読み込み
}
