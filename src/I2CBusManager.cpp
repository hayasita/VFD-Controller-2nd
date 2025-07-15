/**
 * @file I2CBusManager.cpp
 * @author hayasita04@gmail.com
 * @brief I2CBusManagerクラスの実装
 * @version 0.1
 * @date 2025-05-20
 * @copyright Copyright (c) 2025 hayasita04
 * @details
 * I2CBusManagerクラスは、I2Cバスの管理を行うクラスである。
 * このクラスは、I2Cバスの初期化、デバイスのスキャン、デバイス名の取得を行う。
 * I2CBusManagerは、TwoWireクラスを使用してI2C通信を行う。
 * I2Cバスの初期化は、beginメソッドで行われる。
 * スキャンは、scanI2CBusメソッドで行われ、見つかったデバイスのアドレスを返す。
 * デバイス名の取得は、getDeviceNameメソッドで行われ、アドレスに対応するデバイス名を返す。
 * I2CBusManagerは、std::recursive_mutexを使用してI2Cバスへのアクセスを排他制御する。
 * I2CBusManagerは、TwoWireクラスのインスタンスを保持し、I2Cバスの操作を行う。
 */
#include <M5Unified.h>
#include "I2CBusManager.h"
#include "Config.h"

/**
 * @brief I2CBusManagerの初期化
 * @details I2Cバスの初期化を行う。
 */
void I2CBusManager::begin() {
  wire.begin(SDA_PIN, SCL_PIN, I2C_FREQ);
}

/**
 * @brief I2CBusManagerのミューテックスを取得
 * @return std::recursive_mutex& I2Cバスのミューテックス
 */
std::recursive_mutex& I2CBusManager::getMutex() {
  return i2cMutex;
}

/**
 * @brief I2CバスのWireオブジェクトを取得
 * @return TwoWire& I2CバスのWireオブジェクト
 */
TwoWire& I2CBusManager::getWire() {
  return wire;
}

/**
 * @brief I2Cバスのスキャン
 * @param address スキャン開始アドレス（デフォルトは0x00）
 * @param count スキャンするアドレスの数（デフォルトは127）
 * @return スキャン結果のアドレスリスト
 * この関数は、I2Cバス上のデバイスをスキャンし、見つかったデバイスのアドレスを返す。
 * Wireライブラリを使用して、各アドレスに対して通信を試み、応答があったアドレスをリストに追加する。
 */
std::vector<uint8_t> I2CBusManager::scanI2CBus(uint8_t startAddress, uint8_t count) {
  byte error, address;
  int nDevices;
  i2cDevice.clear();  // スキャン結果のアドレスリストをクリア
  std::lock_guard<std::recursive_mutex> lock(i2cMutex); // ミューテックスで排他制御
  
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      i2cDevice.push_back(address);   // DeviceList 追加
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
    else{
//      Serial.println(address,HEX);
//      Serial.println(error);
    }
  }

  return i2cDevice;
}

/**
 * @brief I2Cバスのデバイス名を取得
 * @param address I2Cアドレス
 * @return デバイス名
 * デバイス名は、i2cDeviceNameMapからアドレスに対応する名前を取得する。
 * アドレスが見つからない場合は "Unknown Device" を返す。
 */
std::string I2CBusManager::getDeviceName(uint8_t address) {
  auto it = i2cDeviceNameMap.find(address);
  if (it != i2cDeviceNameMap.end()) {
    return it->second;
  }
  return "Unknown Device";
}    
