#include "ParameterStorage.h"

void ParameterStorage::begin(EepromManager& eeprom) {
  this->eeprom = &eeprom;  // EepromManagerの参照を保持
}

void ParameterStorage::save(uint8_t index, int value) {
  eeprom->writeBytes(PARAM_START_ADDR + index * sizeof(int), &value, sizeof(int));  // パラメータ保存
}

bool ParameterStorage::load(uint8_t index, int& value) {
  return eeprom->readBytes(PARAM_START_ADDR + index * sizeof(int), &value, sizeof(int));  // パラメータ読み込み
}
