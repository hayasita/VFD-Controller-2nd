#include "../../src/EepromRawAccessor.h"
#include <cstring>  // 追加

EepromRawAccessor::EepromRawAccessor(I2CBusManager *busManager)
  : i2cBus(busManager)  // I2CBusManagerの参照を設定
{
}

void EepromRawAccessor::begin(void) {
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());

  EEPROM.begin(EEPROM_SIZE);
  eeprom = (uint8_t*)EEPROM.getDataPtr();  // EEPROMデータポインタを取得
}

/*
bool EepromRawAccessor::writeBytes(int address, const void* data, size_t len) {
  if (address + len > EEPROM_SIZE) return false;  // 範囲外アクセスを防止
  memcpy(eeprom + address, data, len);  // データ書き込み
  return EEPROM.commit();  // 書き込みを確定
}
*/

bool EepromRawAccessor::writeByte(unsigned short address, unsigned char data) {
    // ダミー実装（必要に応じて本実装）
    return true;
}

/*
bool EepromRawAccessor::readBytes(int address, void* data, size_t len) {
  if (address + len > EEPROM_SIZE) return false;  // 範囲外アクセスを防止
  memcpy(data, eeprom + address, len);  // データ読み込み
  return true;
}
*/

bool EepromRawAccessor::readByte(unsigned short address, unsigned char* data) {
    // ダミー実装（必要に応じて本実装）
    if (data) *data = 0;
    return true;
}

bool EepromRawAccessor::sequentialRead(uint16_t eeADR ,uint8_t *data,uint8_t dataNum){
  uint8_t i=0;
  return true;
}
