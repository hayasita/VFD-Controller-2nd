#pragma once

#include "SystemEvent.h"
#include "WiFiManager.h"
#include "TimeManager.h"

class ParameterManager; // 前方参照
#define DISP_KETAMAX  9   // VFD表示桁数

enum class SystemMode {
  Clock,
  WiFiConnected,
  WiFiDisconnected,
  // 他のモード
};

class SystemManager {
public:
  SystemManager() = default;        // コンストラクタ
  ~SystemManager() = default;
  virtual void begin(WiFiManager& wifi, TimeManager& time, ParameterManager& parameter);  // WiFiManagerとTimeManagerの初期化
  virtual void update(SystemEvent event);

  // パラメータ変更通知
  virtual void onParameterChanged(uint8_t index, uint8_t newValue);
  virtual bool setParameterByPrnum(uint8_t prnum, int value);
//  virtual bool setParameterByKey(const std::string& key, int value);
  virtual bool setTimezone(uint8_t zoneData); // タイムゾーン設定
  virtual void setBrDig(uint8_t adj_point,uint8_t brw);     // 輝度情報個別設定
  virtual bool setParameterBrDig(void);                     // 輝度情報個別設定 Pr設定
  virtual bool resetBrDig(void);                            // 輝度情報個別設定 Pr設定値でリセット

  virtual void updateWiFiAutoConnect(void); // WiFi自動接続の更新

  virtual std::string makeSettingJs(void);  // ./setting.jsを生成する

private:
  SystemMode currentMode = SystemMode::Clock;
  SystemMode currentWifiMode = SystemMode::WiFiDisconnected;
  WiFiManager* wifiManager = nullptr;
  TimeManager* timeManager = nullptr;
  ParameterManager* parameterManager = nullptr;  // パラメータ管理クラスへのポインタ

  bool format12h = false;                 // 時刻表示12/24 Pr.0と連動
  uint8_t dispFormat = 0;                 // 表示フォーマット設定 Pr.1と連動
  uint8_t timeDisplayFormat = 0;          // 時刻表示フォーマット設定 Pr.2と連動
  uint8_t dateDisplayFormat = 0;          // 日付表示フォーマット設定 Pr.3と連動
  uint8_t displayEffect = 0;              // 表示効果 Pr.4と連動
  uint8_t fadetimew = 2;                  // クロスフェード時間(1~9) Pr.5と連動
  uint8_t glowInTheBright = 0;            // 全体輝度設定値：明 Pr.6と連動
  uint8_t glowInTheBrightTmp = 0;         // 全体輝度設定値：明（テンポラリ）
  uint8_t glowInTheDark = 0;              // 全体輝度設定値：暗 Pr.7と連動
  uint8_t glowInTheDarkTmp = 0;           // 全体輝度設定値：暗（テンポラリ）
  uint8_t brDig[DISP_KETAMAX] = {0x02};   // 0x02 - 0x0A 表示各桁輝度 Pr.8-16と連動

  bool ntpSet = false;          // NTP設定フラグ  Pr.32と連動
  uint8_t timeZoneAreaId = 0;   // タイムゾーンエリアID Pr.33と連動
  uint8_t timeZoneId = 0;       // タイムゾーンID Pr.34と連動
  uint8_t timeZoneData = 0;     // タイムゾーンデータ Pr.35と連動
  uint8_t autoUpdateHourw = 0;  // 自動更新時刻：時 Pr.36と連動
  uint8_t autoUpdateMinw = 0;   // 自動更新時刻：分 Pr.37と連動

  uint8_t localesId = 0;        // 地域設定 Pr.43と連動
  bool staAutoConnect = false;  // WiFi Station自動接続フラグ Pr.44と連動

  bool isFormat12h() const { return format12h; }            // 12時間表示フォーマットフラグ
  bool isNtpAutoSet() const { return ntpSet; }              // NTP自動設定フラグ
  bool isNtpSet() const { return ntpSet; }                  // NTP設定フラグ
  bool isStaAutoConnect() const { return staAutoConnect; }  // WiFi Station自動接続フラグ
};
