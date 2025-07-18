#include "EepromRawAccessor.h"
#include "I2CBusManager.h"
#include <cstring>

EepromRawAccessor::EepromRawAccessor(I2CBusManager *busManager)
  : i2cBus(busManager)  // I2CBusManagerの参照を設定
{
}

/**
 * @brief EEPROMの初期化
 * I2C EEPROMが接続されていない場合は、ESP32のEEPROMを使用する。
 */
void EepromRawAccessor::begin(void) {
  if(i2cBus->isEepromConnected()){
    Serial.println("I2C EEPROM is connected.");
  } else {
    Serial.println("I2C EEPROM is not connected.");
    EEPROM.begin(EEPROM_SIZE);
    eeprom = (uint8_t*)EEPROM.getDataPtr();  // EEPROMデータポインタを取得
  }

  return;
}

#ifdef DELETE
bool EepromRawAccessor::readBytes(int address, void* data, size_t len) {
  if (address + len > EEPROM_SIZE) return false;  // 範囲外アクセスを防止
  memcpy(data, eeprom + address, len);  // データ読み込み
  return true;
}

bool EepromRawAccessor::writeBytes(int address, const void* data, size_t len) {
  if (address + len > EEPROM_SIZE) return false;  // 範囲外アクセスを防止
  memcpy(eeprom + address, data, len);  // データ書き込み
  return EEPROM.commit();  // 書き込みを確定
}
#endif

/**
 * @brief EEPROMに1バイト書き込み
 * @param address 書き込みアドレス
 * @param data 書き込むデータ
 * @return true 成功、false 失敗
 *  i2c EEPROMが接続されている場合はI2Cから書き込み、接続されていない場合はESP32のEEPROMから書き込む。
 *  範囲外アクセスを防止するため、アドレスが0以上EEPROM_SIZE未満であることを確認する。
 */
bool EepromRawAccessor::writeByte(uint16_t address, uint8_t data) {
  if(address < 0 || address >= EEPROM_SIZE) {
    return false;  // 範囲外アクセスを防止
  }

  if(i2cBus->isEepromConnected()){
    return i2cWriteByte(address, data);   // I2C EEPROMに書き込み
  } else {
    eeprom[address] = data;               // ESP32のEEPROMに書き込み
    return EEPROM.commit();               // 書き込みを確定
  }
}

/**
 * @brief i2c EEPROMに1バイト書き込み
 * @param address 書き込みアドレス
 * @param data 書き込むデータ
 * @return true 成功、false 失敗
 */
bool EepromRawAccessor::i2cWriteByte(uint16_t address, uint8_t data)
{
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());
  uint8_t i2cADR = I2CADR_EEPROM;       // I2Cアドレス
  Wire.beginTransmission(i2cADR);                // i2cアドレス指定
  Wire.write((int)(address >> 8));               // EEPROM内アドレス指定 MSB
  Wire.write((int)(address & 0xFF));             // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(5);                                      // 書き込み完了待ち

  return true;
}

/**
 * @brief EEPROMから1バイト読み込み
 * @param address EEPROM内アドレス
 * @param data 読み込んだデータを格納するポインタ
 * @return true 成功、false 失敗
 *  i2c EEPROMが接続されている場合はI2Cから読み込み、接続されていない場合はESP32のEEPROMから読み込む。
 *  範囲外アクセスを防止するため、アドレスが0以上EEPROM_SIZE未満であることを確認する。
 */
bool EepromRawAccessor::readByte(uint16_t address, uint8_t *data) {
  if(address < 0 || address >= EEPROM_SIZE) {
    return false;  // 範囲外アクセスを防止
  }

  if(i2cBus->isEepromConnected()){
    return i2cReadByte(address, data);  // I2C EEPROMから読み込み
  } else {
    *data = eeprom[address];            // ESP32のEEPROMから読み込み
    return true;
  }
}

/**
 * @brief i2c EEPROMから1バイト読み込み
 * @param address EEPROM内アドレス
 * @param data 読み込んだデータを格納するポインタ
 * @return true 成功、false 失敗
 */
bool EepromRawAccessor::i2cReadByte(uint16_t address, uint8_t *data)
{
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());
  uint8_t i2cADR = I2CADR_EEPROM;       // I2Cアドレス

  Wire.beginTransmission(i2cADR);       // i2cアドレス指定
  Wire.write((int)(address >> 8));      // EEPROM内アドレス指定 MSB
  Wire.write((int)(address & 0xFF));    // LSB
  Wire.endTransmission();
  Wire.requestFrom(i2cADR, 1);          // 1バイトデータリクエスト
  while (Wire.available()) {
    *data = Wire.read();                // データ受信
  }

  return true;  // 成功
}

/**
 * @brief EEPROMから複数バイト読み込み
 * @param eeADR EEPROM内アドレス
 * @param data 読み込んだデータを格納するバッファ
 * @param dataNum 読み込むデータのバイト数
 * @return true 成功、false 失敗
 * この関数は、指定されたEEPROM内アドレスから複数バイトのデータを読み込む。
 * データは指定されたバッファに格納され、読み込みが成功した場合はtrueを返す。
 * 失敗した場合はfalseを返す。
 * I2C EEPROMが接続されている場合はI2Cから読み込み、接続されていない場合はESP32のEEPROMから読み込む。
 */
bool EepromRawAccessor::sequentialRead(uint16_t eeADR ,uint8_t *data,uint8_t dataNum){
  uint8_t i=0;
  if (dataNum == 0) return false;  // 読み込むデータ数が0の場合は失敗
  if (data == nullptr) return false;  // データポインタがnullptrの場合は失敗 
  if (eeADR + dataNum > EEPROM_SIZE) return false;  // 範囲外アクセスを防止

  if(i2cBus->isEepromConnected()){
    return i2cSequentialRead(I2CADR_EEPROM, eeADR, data, dataNum);  // I2C EEPROMから読み込み
  } else {
    memcpy(data, eeprom + eeADR, dataNum);  // ESP32のEEPROMから読み込み
    return true;
  }

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
bool EepromRawAccessor::i2cSequentialRead(uint8_t i2cADR, uint16_t eeADR ,uint8_t *data,uint8_t dataNum)
{
  uint8_t i=0;
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
