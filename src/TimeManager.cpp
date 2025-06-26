/**
 * @file RTCManager.cpp
 * @author hayasita04@gmail.com
 * @brief TimeManagerクラスの実装
 * @version 0.1
 * @date 2025-05-20
 * @copyright Copyright (c) 2025 hayasita04
 * @details
 * TimeManagerクラスは、時刻の管理を行うクラスである。
 */
#include "TimeManager.h"
#include <sntp.h>
#include <esp_sntp.h>

/**
 * @brief TimeManagerの初期化
 * @details
 * TimeManagerは、時刻の管理を行うクラスである。
 * RTCデバイスを使用するため、RTCManagerクラスを引数に取る。
 */
bool TimeManager::begin(RTCManager* rtcManager) {
  rtc = rtcManager;
  return rtc != nullptr;
}

/**
 * @brief システム時刻を取得
 * @return 現在のシステム時刻（UNIX時間）
 * @note UNIX時間は、1970年1月1日00:00:00 UTCからの経過秒数である。
 *       これを使用することで、システム時刻を簡単に取得することができる。
 */
time_t TimeManager::getSystemTime() {
  return time(nullptr);
}

/**
 * @brief システム時刻を取得
 * @return 現在のシステム時刻（struct tm形式）
 * @note struct tmは、年、月、日、時、分、秒を持つ構造体である。
 *       これを使用することで、システム時刻を簡単に取得することができる。
 */
struct tm TimeManager::getSystemTimeStruct() {
  time_t now;
  time(&now);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  return timeinfo;
}

/**
 * @brief RTC時刻を取得
 * @return 現在のRTC時刻（struct tm形式）
 * @note RTCは、リアルタイムクロックであり、システム時刻とは独立して動作する。
 */
struct tm TimeManager::getRtcTimeStruct()
{
  if (!rtc) return {};

  DateTime dt = rtc->now();
  struct tm t_st;
  t_st.tm_year = dt.year() - 1900;
  t_st.tm_mon  = dt.month() - 1;
  t_st.tm_mday = dt.day();
  t_st.tm_hour = dt.hour();
  t_st.tm_min  = dt.minute();
  t_st.tm_sec  = dt.second();
  t_st.tm_isdst = 0;

  return t_st;

}

/**
 * @brief ローカル時刻を取得
 * @return 現在のローカル時刻（struct tm形式）
 * @note ローカル時刻は、システムのタイムゾーンに基づいて計算される。
 *       タイムゾーンは事前に設定されている必要がある。
 *       これを使用することで、ローカル時刻を簡単に取得することができる。
 */
struct tm TimeManager::getLocalTimeStruct()
{
  // 現在のUTC時刻を取得
  time_t now = time(nullptr);

  // JST（ローカル時刻）に変換
  struct tm *local = localtime(&now);
  return *local;
}

/**
 * @brief システム時刻を設定
 * @param t 設定する時刻（UNIX時間）
 * @note UNIX時間は、1970年1月1日00:00:00 UTCからの経過秒数である。
 *       これを使用することで、システム時刻を簡単に設定することができる。
 */
void TimeManager::setSystemTime(time_t t) {
    struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
    settimeofday(&tv, nullptr);
}

/**
 * @brief RTCからシステム時刻を設定
 * @param tz タイムゾーン情報
 * @note RTCから取得した時刻をシステム時刻として設定する。
 *       RTCは、RTCManagerクラスを使用して取得する。
 */
void TimeManager::setSystemTimeFromRtc(struct timezone* tz)
{
  if (!rtc) return;
  DateTime dt = rtc->now();

  struct tm t_st;
  t_st.tm_year = dt.year() - 1900;
  t_st.tm_mon  = dt.month() - 1;
  t_st.tm_mday = dt.day();
  t_st.tm_hour = dt.hour();
  t_st.tm_min  = dt.minute();
  t_st.tm_sec  = dt.second();
  t_st.tm_isdst = 0;

  timeval now;
  char *oldtz = getenv("TZ");
  setenv("TZ", "GMT0", 1);
  tzset(); // Workaround for https://github.com/espressif/esp-idf/issues/11455
  now.tv_sec = mktime(&t_st);
  if (oldtz){
    setenv("TZ", oldtz, 1);
  } else {
    unsetenv("TZ");
  }
  now.tv_usec = 0;
  settimeofday(&now, tz);

  return;
}

/**
 * @brief 手動でシステム時刻を設定
 * @param year 年
 * @param month 月
 * @param day 日
 * @param hour 時
 * @param minute 分
 * @param second 秒
 * @note 手動でシステム時刻を設定するための関数である。
 *       引数には、年、月、日、時、分、秒を指定する。
 */
void TimeManager::setSystemTimeFromManually(int year, int month, int day, int hour, int minute, int second) {
  struct tm t;
  t.tm_year = year - 1900;
  t.tm_mon  = month - 1;
  t.tm_mday = day;
  t.tm_hour = hour;
  t.tm_min  = minute;
  t.tm_sec  = second;
  t.tm_isdst = 0;

  setSystemTime(mktime(&t));

  return;
}

/**
 * @brief タイムゾーンを更新
 * @param tzParam タイムゾーンのパラメータ（例: "JST-9"）
 * @note タイムゾーンを更新するための関数である。
 */
void TimeManager::updateTimeZone(const std::string& tzParam) {
  setenv("TZ", tzParam.c_str(), 1);
  tzset();

  return;
}

/**
 * @brief システム時刻をRTCに保存
 * @note システム時刻をRTCに保存するための関数である。
 *      RTCは、RTCManagerクラスを使用して取得する。
 *      RTCの時刻は、DateTime型で取得する。
 *      DateTime型は、年、月、日、時、分、秒を持つクラスである。
 */
void TimeManager::updateRTCFromSystemTime() {
  if (!rtc) return;
  Serial.println("updateRTCFromSystemTime");

  time_t timeinfo = getSystemTime();

  DateTime dt(timeinfo);
  Serial.printf("SystemTime: %04d-%02d-%02d %02d:%02d:%02d\n",
    dt.year(), dt.month(), dt.day(),
    dt.hour(), dt.minute(), dt.second()
  );

  rtc->adjust(dt);

  DateTime now = rtc->now();
  Serial.printf("RTC updated: %04d-%02d-%02d %02d:%02d:%02d\n",
                now.year(), now.month(), now.day(),
                now.hour(), now.minute(), now.second());

  return;
}

/**
 * @brief SNTP同期の設定
 * @note SNTP同期の設定を行うための関数である。
 */
void TimeManager::configureSNTP(void) {
  const char* timeZone  = "UTC";              // タイムゾーン設定 : "UTC"
  const char* ntpServer = "pool.ntp.org";

  Serial.println("-- SntpCont::init --");
//  Serial.println(confDat.getTimeZoneData());
//  if(confDat.getNtpset() == 1){
  if(true){
    sntp_set_sync_mode ( SNTP_SYNC_MODE_IMMED );    // 同期モード設定　すぐに更新
    sntp_set_sync_interval(1*3600*1000); // 1 hours
//    sntp_set_sync_interval(2*60*1000); // 120Sec
    configTzTime(timeZone, ntpServer);

    // status
    Serial.printf("setup: SNTP sync mode = %d (0:IMMED 1:SMOOTH)\n", sntp_get_sync_mode());
    Serial.printf("setup: SNTP sync status = %d (1:SNTP_SYNC_STATUS_RESET 2:SNTP_SYNC_STATUS_IN_PROGRESS 3:SNTP_SYNC_STATUS_COMPLETED)\n", sntp_get_sync_status());
    Serial.printf("setup: SNTP sync interval = %dms\n", sntp_get_sync_interval());

    // set sntp callback
    sntp_set_time_sync_notification_cb(TimeManager::SntpTimeSyncNotificationCallback);
  }

//  sntpCompleted = false;    // SNTP同期完了フラグ初期化

  return;
}

/**
 * @brief シングルトンインスタンスを設定
 * @param inst TimeManagerのインスタンス
 * @note
 * この関数は、シングルトンパターンを使用してTimeManagerのインスタンスを設定するための関数である。
 */
void TimeManager::setInstance(TimeManager* inst) {
  s_instance = inst;
}
TimeManager* TimeManager::s_instance = nullptr;

/**
 * @brief SNTP同期完了コールバック
 * @param tv 時刻情報
 * @note
 * この関数は、SNTP同期が完了したときに呼び出されるコールバック関数である。
 */
void TimeManager::SntpTimeSyncNotificationCallback(struct timeval *tv) {
  Serial.println("SNTP time sync completed");

  if(s_instance && s_instance->sntpSyncCallback) {
    s_instance->sntpSyncCallback();
  }

  if (s_instance) s_instance->updateRTCFromSystemTime();  // RTCに時刻を設定

  return;
}

void TimeManager::onSntpSync(std::function<void()> callback) {
  sntpSyncCallback = callback;
}
