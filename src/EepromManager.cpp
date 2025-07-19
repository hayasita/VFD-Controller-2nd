#include <sstream>
#include <iomanip>
#include <cstring>
#include "Config.h"
#include "EepromManager.h"

EepromManager::EepromManager(I2CBusManager *busManager)
  : i2cBus(busManager)  // I2CBusManagerの参照を設定
  , rawAccessor(busManager)  // EepromRawAccessorを初期化
{
}

/**
 * @brief EEPROM管理クラスの初期化
 * @param busManager I2Cバス管理クラスの参照
 * この関数は、I2Cバスを初期化し、EEPROMの低レベルアクセスを行うEepromRawAccessorを初期化します。
 * i2cBusはI2CBusManagerのインスタンスを参照し、EEPROMへのアクセスを排他制御するためのミューテックスを使用します。
 * この関数は、EEPROMの初期化を行い、I2Cバスの設定を行います。
 */
void EepromManager::begin(void) {
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());

  rawAccessor.begin();  // EepromRawAccessorを初期化
}

/**
 * @brief EEPROMアクセス用のミューテックスを取得
 * @return std::recursive_mutex& EEPROM専用のミューテックス
 */
std::recursive_mutex& EepromManager::getMutex() {
  return eepromMutex;  // EEPROM専用のミューテックスを返す
}

#ifdef DELETE
/**
 * @brief バイト単位でデータを書き込む
 * @param address 書き込み開始アドレス
 * @param data 書き込むデータのポインタ
 * @param len 書き込むデータの長さ
 * @return true 成功、false 失敗
 */
bool EepromManager::writeBytes(int address, const void* data, size_t len) {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  if (address < 0 || address + len > EEPROM_MAX_ADDRESS) {
    return false;  // 範囲外アクセスを防止
  }
  return rawAccessor.writeBytes(address, data, len);  // データ書き込み
}
#endif

/**
 * @brief データを1バイト書き込む
 * @param address 書き込みアドレス
 * @param data 書き込むデータ
 * @return true 成功、false 失敗
 */
bool EepromManager::writeByte(uint16_t address, const uint8_t data) {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  if (address < 0 || address >= EEPROM_MAX_ADDRESS) {
    return false;  // 範囲外アクセスを防止
  }
  return rawAccessor.writeByte(address, data);  // 単一バイトのデータ書き込み
}

#ifdef DELETE
/**
 * @brief バイト単位でデータを読み込む
 * @param address 読み込み開始アドレス
 * @param data 読み込んだデータを格納するバッファ
 * @param len 読み込むデータの長さ
 * @return true 成功、false 失敗
 */
bool EepromManager::readBytes(int address, void* data, size_t len) {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  if (address < 0 || address + len > EEPROM_MAX_ADDRESS) {
    return false;  // 範囲外アクセスを防止
  }
  return rawAccessor.readBytes(address, data, len);  // データ読み込み
}
#endif

/**
 * @brief データを1バイト読み出す
 * @param address 読み出しアドレス
 * @param data 読み出したデータを格納するポインタ
 * @return true 成功、false 失敗
 */
bool EepromManager::readByte(uint16_t address, uint8_t *data) {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  if (address < 0 || address >= EEPROM_MAX_ADDRESS) {
    return false;  // 範囲外アクセスを防止
  }
  
  return rawAccessor.readByte(address, data);
}

/**
 * @brief i2c EEPROMから複数バイト読み込み
 * @param address 読み込み開始アドレス
 * @param data 読み込んだデータを格納するバッファ
 * @param len 読み込むデータの長さ
 * @return true 成功、false 失敗
 */
bool EepromManager::readMultipleBytes(int address, uint8_t *data, size_t len) {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  return rawAccessor.sequentialRead( address, data, len);
}

/**
 * @brief EEPROMデータをダンプする
 * @param address 開始アドレス
 * @param len ダンプするデータの長さ（デフォルトは16バイト）
 * @return ダンプ結果の文字列
 */
std::string EepromManager::dumpEepromData(uint16_t address, uint16_t len)
{
  std::ostringstream oss;
  uint16_t startADR = address;
  uint16_t endADR = std::min<uint16_t>(address + len - 1, EEPROM_MAX_ADDRESS);
  uint8_t datasq[16];
  char ascii[17]; // 16文字+終端
  ascii[16] = '\0';

  std::lock_guard<std::recursive_mutex> lock(getMutex());

  for (uint16_t ADR = startADR; ADR <= endADR; ADR += 16) {
    size_t bytesToRead = std::min<size_t>(16, endADR - ADR + 1);
    memset(datasq, 0, sizeof(datasq));
    memset(ascii, ' ', 16);

    if (!readMultipleBytes(ADR, datasq, bytesToRead)) {
      oss << std::setw(4) << std::setfill('0') << std::hex << ADR << "  <Read Error>\n";
      continue;
    }

    if ((ADR & 0x00FF) == (startADR % 0x100)) {
      oss << "\nADDR  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ASCII CHARACTER\n";
    }

    oss << std::setw(4) << std::setfill('0') << std::hex << ADR << " ";
    for (size_t j = 0; j < 16; ++j) {
      if (j < bytesToRead) {
        oss << " " << std::setw(2) << std::setfill('0') << std::hex << (int)datasq[j];
        ascii[j] = (datasq[j] >= 0x20 && datasq[j] < 0x7F) ? datasq[j] : '.';
      } else {
        oss << "   ";
        ascii[j] = ' ';
      }
    }
    oss << " ";
    oss.write(ascii, 16);
    oss << "\n";
  }

  return oss.str();
}
