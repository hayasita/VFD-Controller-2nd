/**
 * @file RTCManager.cpp
 * @author hayasita04@gmail.com
 * @brief RTCManagerクラスの実装
 * @version 0.1
 * @date 2025-05-20
 * @copyright Copyright (c) 2025 hayasita04
 * @details
 * RTCManagerクラスは、RTCデバイスの管理を行うクラスである。
 * このクラスは、I2Cバスを使用してRTCデバイスにアクセスする。
 * RTCデバイスの種類は、DS1307、DS3231、M5RTCのいずれかとなる。
 * 
 */
#include <M5Unified.h>
#include <RTClib.h>
#include "RTCManager.h"

RTCManager::RTCManager(I2CBusManager* busManager)
  : i2cBus(busManager), rtc1307(), rtc3231()  // I2CBusManagerの参照を設定
{
  return;
}
/**
 * @brief RTCManagerの初期化
 * @details
 * RTCManagerは、RTCデバイスの管理を行うクラスである。
 * このクラスは、I2Cバスを使用してRTCデバイスにアクセスする。
 * RTCデバイスの種類は、DS1307、DS3231、M5RTCのいずれかとなる。
 * 
 */
bool RTCManager::begin(void)
{
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());

  bool found = false;

  if (M5.Rtc.isEnabled()) {
    type = RTCType::M5RTC;
    found = true;
//    M5.Rtc.setDateTime( { { 2021, 12, 31 }, { 12, 34, 56 } } );
  }
  else if (rtc3231.begin(&i2cBus->getWire()) && !rtc3231.lostPower()) {
    type = RTCType::DS3231;
    found = true;
  } else if (rtc1307.begin(&i2cBus->getWire()) && rtc1307.isrunning()) {
    type = RTCType::DS1307;
    found = true;
  } else {
    type = RTCType::None;
  }

  return found;
}

/**
 * @brief RTCが動作しているか確認
 * @return true: 動作中, false: 動作していない
 * @note RTCの種類によって動作確認の方法が異なります。
 *       M5RTCの場合は、M5.Rtc.isEnabled()を使用して動作確認しする。
 *      DS1307の場合は、RTCライブラリのisrunning()メソッドを使用する。
 *      DS3231の場合は、RTCライブラリのlostPower()メソッドを使用して動作確認する。
 *      それぞれのRTCの動作確認方法は、RTCライブラリのドキュメントを参照すること。
 *      なお、RTCが動作していない場合は、RTCType::Noneを返す。
 *      RTCが動作している場合は、RTCType::M5RTC, RTCType::DS1307, RTCType::DS3231のいずれかを返す。
 */
bool RTCManager::isRunning() {
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());
  switch (type) {
    case RTCType::M5RTC: return M5.Rtc.isEnabled();
    case RTCType::DS1307: return rtc1307.isrunning();
    case RTCType::DS3231: return !rtc3231.lostPower();
    default: return false;
  }
}

/**
 * @brief RTCの現在時刻を取得
 * @return 現在時刻
 * @note M5RTCの場合は、M5.Rtc.getDateTime()を使用して時刻を取得する。
 *      DS1307とDS3231の場合は、RTCライブラリのnow()メソッドを使用して時刻を取得する。
 *      現在時刻は、DateTime型で返される。
 */
DateTime RTCManager::now() {
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());
  switch (type) {
    case RTCType::M5RTC:{
      return toDateTime(M5.Rtc.getDateTime());  // GMT
    }
    case RTCType::DS1307: return rtc1307.now();
    case RTCType::DS3231: return rtc3231.now();
    default: return DateTime(2000, 1, 1, 0, 0, 0);
  }
}

/**
 * @brief RTCの時刻を設定
 * @param dt 設定する時刻
 * @note M5RTCの場合は、M5.Rtc.setDateTime()を使用して時刻を設定する。
 *      DS1307とDS3231の場合は、RTCライブラリのadjust()メソッドを使用する。
 */
void RTCManager::adjust(const DateTime& dt) {
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());
  switch (type) {
    case RTCType::M5RTC: M5.Rtc.setDateTime(toRtcDateTime(dt)); break;
    case RTCType::DS1307: rtc1307.adjust(dt); break;
    case RTCType::DS3231: rtc3231.adjust(dt); break;
    default: break;
  }
} 

/**
 * @brief DS3231の温度を取得
 * @return 温度（摂氏）
 * @note DS3231以外のRTCの場合は、NANを返す。
 *      DS3231の温度は、RTCライブラリのgetTemperature()メソッドを使用して取得する。
 *      温度は摂氏で返す。
 */
float RTCManager::getTemperature() {
  std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());
  if (type == RTCType::DS3231) {
    return rtc3231.getTemperature();
  } else {
    return NAN;
  }
}

/**
 * @brief RTCの時刻をDateTime型に変換
 * @param dt RTCの時刻
 * @return DateTime型の時刻
 * @note RTCの時刻は、m5::rtc_datetime_t型で取得される。
 *       これをDateTime型に変換するためのヘルパー関数である。
 *      DateTime型は、RTClibライブラリのDateTimeクラスを使用している。
 *      DateTime型は、年、月、日、時、分、秒を持つクラスである。
 *      m5::rtc_datetime_t型は、年、月、日、時、分、秒を持つ構造体である。
 *      これらの型は、RTCの時刻を扱うために使用される。
 */
DateTime RTCManager::toDateTime(const m5::rtc_datetime_t dt)
{
  return DateTime(dt.date.year,  dt.date.month, dt.date.date, dt.time.hours, dt.time.minutes, dt.time.seconds);
}

/**
 * @brief DateTime型の時刻をm5::rtc_datetime_t型に変換
 * @param dt DateTime型の時刻
 * @return m5::rtc_datetime_t型の時刻
 */
m5::rtc_datetime_t RTCManager::toRtcDateTime(const DateTime& dt) {
  m5::rtc_datetime_t rtcDt;
  rtcDt.date.year     = dt.year();
  rtcDt.date.month    = dt.month();
  rtcDt.date.date     = dt.day();
  rtcDt.time.hours    = dt.hour();
  rtcDt.time.minutes  = dt.minute();
  rtcDt.time.seconds  = dt.second();
  return rtcDt;
}
