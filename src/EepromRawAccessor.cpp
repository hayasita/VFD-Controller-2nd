#include "EepromRawAccessor.h"
#include <cstring>  // 追加

void EepromRawAccessor::begin(I2CBusManager& busManager) {
  i2cBus = &busManager;
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());

  EEPROM.begin(EEPROM_SIZE);
  eeprom = (uint8_t*)EEPROM.getDataPtr();  // EEPROMデータポインタを取得
}

bool EepromRawAccessor::writeBytes(int address, const void* data, size_t len) {
  if (address + len > EEPROM_SIZE) return false;  // 範囲外アクセスを防止
  memcpy(eeprom + address, data, len);  // データ書き込み
  return EEPROM.commit();  // 書き込みを確定
}

bool EepromRawAccessor::readBytes(int address, void* data, size_t len) {
  if (address + len > EEPROM_SIZE) return false;  // 範囲外アクセスを防止
  memcpy(data, eeprom + address, len);  // データ読み込み
  return true;
}

/**
 * @brief i2c EEPROMから複数バイト読み込み
 * @param i2cADR I2Cアドレス
 * @param eeADR EEPROM内アドレス
 * @param data 読み込んだデータを格納するバッファ
 * @param dataNum 読み込むデータのバイト数
 * @return true 成功、false 失敗
 * この関数は、指定されたI2CアドレスとEEPROM内アドレスから複数バイトのデータを読み込む。
 * データは指定されたバッファに格納され、読み込みが成功した場合はtrueを返す。
 * 失敗した場合はfalseを返す。
 */
bool EepromRawAccessor::sequentialRead(uint8_t i2cADR, uint16_t eeADR ,uint8_t *data,uint8_t dataNum){
  uint8_t i=0;
  if (dataNum == 0) return false;  // 読み込むデータ数が0の場合は失敗
  if (data == nullptr) return false;  // データポインタがnullptrの場合は失敗 
//  if (eeromConent != 0){
//    Serial.println("I2C EEPROM device not found.");
//    return;
//  }

  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());
  Wire.beginTransmission(i2cADR);               // i2cアドレス指定
  Wire.write((int)(eeADR >> 8));                // EEPROM内アドレス指定 MSB
  Wire.write((int)(eeADR & 0xFF));              // LSB
  Wire.endTransmission();
  Wire.requestFrom(i2cADR, dataNum);            // 1バイトデータリクエスト
  while (Wire.available()) {                    //
    data[i] = Wire.read();
//    Serial.print(data[i] >> 4, HEX);                // 上位4ビットを16表示
//    Serial.print(data[i] & 0x0F, HEX);              // 下位4ビット
//    Serial.print(" ");                           // データ間にスペース入れ
    i++;
  }
//  Serial.println();                         // データ受信

  return true;
}
