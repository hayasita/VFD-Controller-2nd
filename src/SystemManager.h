#pragma once

#include "SystemEvent.h"
#include "WiFiManager.h"
#include "TimeManager.h"
#include "TerminalInputManager.h" // 端子入力管理クラス
#include "LedController.h"        // LED制御クラス

class ParameterManager; // 前方参照
#define DISP_KETAMAX  9   // VFD表示桁数

enum class ParamIndex : uint8_t {
  Format12h = 0,          // 12時間表示フォーマット Pr.0
  DispFormat = 1,         // 表示フォーマット Pr.1
  TimeDisplayFormat = 2,  // 時刻表示フォーマット Pr.2
  DateDisplayFormat = 3,  // 日付表示フォーマット Pr.3
  DisplayEffect = 4,      // 表示効果 Pr.4
  FadeTime = 5,           // クロスフェード時間 Pr.5
  GlowInTheBrightTmp = 6, // 全体輝度設定値：明（テンポラリ） Pr.6
  GlowInTheDarkTmp = 7,   // 全体輝度設定値：暗（テンポラリ） Pr.7
  BrDig0 = 8,             // 表示桁0の輝度 Pr.8
  BrDig1 = 9,             // 表示桁1の輝度 Pr.9
  BrDig2 = 10,            // 表示桁2の輝度 Pr.10
  BrDig3 = 11,            // 表示桁3の輝度 Pr.11
  BrDig4 = 12,            // 表示桁4の輝度 Pr.12
  BrDig5 = 13,            // 表示桁5の輝度 Pr.13
  BrDig6 = 14,            // 表示桁6の輝度 Pr.14
  BrDig7 = 15,            // 表示桁7の輝度 Pr.15
  BrDig8 = 16,            // 表示桁8の輝度 Pr.16

  NtpSet = 32,            // Pr.32: SNTP設定：SNTP使用
  TimeZoneAreaId = 33,    // Pr.33: SNTP設定：タイムゾーンエリアID
  TimeZoneId = 34,        // Pr.34: SNTP設定：タイムゾーンID
  TimeZoneData = 35,      // Pr.35: SNTP設定：タイムゾーン

  LocalesId = 43,         // Pr.43: 地域設定
  StaAutoConnect = 44     // Pr.44: WiFi Station 設定：STA自動接続有効
};

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
  virtual void initDependencies(WiFiManager& wifi, TimeManager& time, ParameterManager& parameter, TerminalInputManager& terminal, LedController& builtInLed, LedController& externalLed);  // 依存関係の初期化
  virtual void begin(void);         // システム起動処理
  virtual void update(void);

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
  TerminalInputManager* terminalInputManager = nullptr;    // 端子入力管理
  LedController* builtInLedCtrl = nullptr;      // 内蔵LED制御クラスへのポインタ
  LedController* externalLedCtrl = nullptr;     // 外部LED制御クラスへのポインタ

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
