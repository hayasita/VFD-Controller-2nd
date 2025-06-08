/**
 * @file TimeManager.h
 * 
 */
#pragma once

#include <M5Unified.h>
#include <time.h>
#include "RTCManager.h"

class TimeManager {
public:
  bool begin(RTCManager* rtcManager);         // RTCManagerの初期化
  time_t getSystemTime();                     // システム時刻を取得（UNIX時間）
  struct tm getSystemTimeStruct();            // システム時刻を取得（struct tm形式）
  void setSystemTime(time_t t);               // システム時刻を設定
  void setSystemTimeFromRtc(struct timezone* tz);     // RTCからシステム時刻を設定
  void setSystemTimeFromManually(int year, int month, int day, int hour, int minute, int second); // 手動でシステム時刻を設定
  void updateRTCFromSystemTime();             // システム時刻をRTCに保存

  void configureSNTP(void);                   // SNTP同期設定
  static void setInstance(TimeManager* inst); // インスタンス設定
  static void SntpTimeSyncNotificationCallback(struct timeval *tv);   // SNTP同期完了コールバック(system)
  void onSntpSync(std::function<void()> callback);                    // SNTP同期完了コールバック関数を設定
private:
  RTCManager* rtc = nullptr;              // RTCManagerインスタンス
  static TimeManager* s_instance;         // シングルトンインスタンス

  std::function<void()> sntpSyncCallback; // SNTP同期完了コールバック関数
};
