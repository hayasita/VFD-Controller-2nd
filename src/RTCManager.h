/**
 * @file RTCManager.h
 *   
 */
#pragma once

#include <M5Unified.h>
#include <RTClib.h>
#include "I2CBusManager.h"

/**
 * @brief RTCの種類を表す列挙型
 * @details
 * RTCTypeは、RTCデバイスの種類を表す列挙型。
 *  RTCデバイスの種類は、DS1307、DS3231、M5RTCのいずれかとなる。
 */
enum class RTCType {
  None,
  M5RTC,
  DS1307,
  DS3231
};

/**
 * @brief RTCManagerクラスの実装
 * @details
 * RTCManagerクラスは、RTCデバイスの管理を行うクラスである。
 * このクラスは、I2Cバスを使用してRTCデバイスにアクセスする。
 * RTCデバイスの種類は、DS1307、DS3231、M5RTCのいずれかとなる。
 */
class RTCManager {
public:
  bool begin(I2CBusManager& busManager);        // RTCの初期化（I2CBusManager経由でI2Cを使用）
  bool isRunning();                             // RTCが動作しているか確認
  DateTime now();                               // 現在時刻を取得
  void adjust(const DateTime& dt);              // RTCの時刻を設定
  float getTemperature();                       // DS3231の温度を取得（摂氏）
  RTCType getRTCType() const { return type; }   // RTCの種類を取得

private:
  I2CBusManager* i2cBus = nullptr;  // I2Cバスマネージャ
  RTC_DS1307 rtc1307;               // DS1307 RTC
  RTC_DS3231 rtc3231;               // DS3231 RTC
  RTCType type = RTCType::None;     // RTCの種類

  DateTime toDateTime(const m5::rtc_datetime_t dt);     // RTCの時刻をDateTime型に変換
  m5::rtc_datetime_t toRtcDateTime(const DateTime& dt); // DateTime型の時刻をRTCの形式に変換
};
