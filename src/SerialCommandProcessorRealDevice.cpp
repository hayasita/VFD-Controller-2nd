/**
 * @file monitor_real_serial.cpp
 * @author your name (you@domain.com)
 * @brief シリアルモニタ用送受信実処理
 * @version 0.1
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <M5Unified.h>
#include "SerialCommandProcessorRealDevice.h"

/**
 * @brief Construct a new Real Serial Monitor I O:: Real Serial Monitor I O object
 * 
 */
RealMonitorDeviseIo::RealMonitorDeviseIo(void){
  buffer.clear();
};

/**
 * @brief シリアル受信処理
 * 
 * @return std::string 
 */
std::string RealMonitorDeviseIo::rsv() {
  std::string ret;

  if(Serial.available() > 0){
    char data = Serial.read();      // データを読み取り
    Serial.print(data);             // エコーバック出力

    if(data != '\n'){
      // 入力文字追加
      buffer.push_back(data);
      ret.clear();
    }
    else{
      // 入力確定
      ret = buffer;
      buffer.clear();
    }
  }
  return ret;
}

/**
 * @brief シリアル送信処理
 * 
 * @param data 
 * @return uint8_t 
 */
uint8_t RealMonitorDeviseIo::send(std::string data) {
  uint8_t ret;

  String arduinoStr = String(data.c_str());
  Serial.print(arduinoStr);

  return ret;
}
