/**
 * @file TimeManager.h
 * 
 */
#pragma once

//#include <M5Unified.h>
#include <time.h>
#include <functional>
#include <sys/time.h> // struct timezoneを使用するために必要
#include <string>

#ifdef UNIT_TEST
// ...モック定義...
class RTCManager{}; // RTCManagerの前方宣言（実際のRTCManagerクラスは別ファイルに定義されていると仮定）
#else
// ...本来の定義...
#include "RTCManager.h"
#endif

class AbstractTimeManager {
public:
  virtual ~AbstractTimeManager() = default;
  virtual void updateTimeZone(const std::string& tzParam) = 0;
  // 必要なら他の純粋仮想関数もここに追加
};

/**
 * @brief 時間管理クラス
 * 
 * TimeManagerは、RTC（リアルタイムクロック）を使用してシステム時刻を管理するクラスである。
 * システム時刻の取得、設定、RTCとの同期、SNTP同期などの機能を提供する。
 */
class TimeManager : public AbstractTimeManager {
public:
  bool begin(RTCManager* rtcManager);         // RTCManagerの初期化
  time_t getSystemTime();                     // システム時刻を取得（UNIX時間）
  struct tm getSystemTimeStruct();            // システム時刻を取得（struct tm形式）
  struct tm getRtcTimeStruct();               // RTC時刻を取得（struct tm形式）
  struct tm getLocalTimeStruct();             // ローカル時刻を取得（struct tm形式）
  void setSystemTime(time_t t);               // システム時刻を設定
  void setSystemTimeFromRtc(struct timezone* tz);     // RTCからシステム時刻を設定
  void setSystemTimeFromManually(int year, int month, int day, int hour, int minute, int second); // 手動でシステム時刻を設定
  void updateRTCFromSystemTime();             // システム時刻をRTCに保存
  virtual void updateTimeZone(const std::string& tzParam) override;    // タイムゾーンを更新

  void configureSNTP(void);                   // SNTP同期設定
  static void setInstance(TimeManager* inst); // インスタンス設定
  static void SntpTimeSyncNotificationCallback(struct timeval *tv);   // SNTP同期完了コールバック(system)
  void onSntpSync(std::function<void()> callback);                    // SNTP同期完了コールバック関数を設定
private:
  RTCManager* rtc = nullptr;              // RTCManagerインスタンス
  static TimeManager* s_instance;         // シングルトンインスタンス

  std::function<void()> sntpSyncCallback; // SNTP同期完了コールバック関数
};
