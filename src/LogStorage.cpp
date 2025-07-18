#include "LogStorage.h"

void LogStorage::begin(EepromManager& eeprom) {
  this->eeprom = &eeprom;  // EepromManagerの参照を保持
}

bool LogStorage::write(int address, const char* message, size_t len) {
//  return eeprom->writeBytes(address, message, len);  // ログの書き込み
  return true;
}

bool LogStorage::read(int address, char* buffer, size_t len) {
//  return eeprom->readBytes(address, buffer, len);  // ログの読み込み
  return true;
}
