#pragma once

#include "SystemEvent.h"
#include "WiFiManager.h"
#include "TimeManager.h"

class ParameterManager; // 前方参照

enum class SystemMode {
  Clock,
  WiFiConnected,
  WiFiDisconnected,
  // 他のモード
};

class SystemManager {
public:
  void begin(WiFiManager& wifi, TimeManager& time, ParameterManager& parameter);  // WiFiManagerとTimeManagerの初期化
  void update(SystemEvent event);

  // パラメータ変更通知
  void onParameterChanged(uint8_t index, uint8_t newValue);
  bool setParameterByKey(const std::string& key, int value);

private:
  SystemMode currentMode = SystemMode::Clock;
  SystemMode currentWifiMode = SystemMode::WiFiDisconnected;
  WiFiManager* wifiManager = nullptr;
  TimeManager* timeManager = nullptr;
  ParameterManager* parameterManager = nullptr;  // パラメータ管理クラスへのポインタ

  bool format12h = false;       // 12時間表示フラグ Pr.1と連動

  bool ntpSet = false;          // NTP設定フラグ  Pr.30と連動
  bool staAutoConnect = false;  // WiFi Station自動接続フラグ Pr.44と連動

  void updateWiFiAutoConnect(void);

  bool isFormat12h() const { return format12h; }            // 12時間表示フォーマットフラグ
  bool isNtpAutoSet() const { return ntpSet; }              // NTP自動設定フラグ
  bool isNtpSet() const { return ntpSet; }                  // NTP設定フラグ
  bool isStaAutoConnect() const { return staAutoConnect; }  // WiFi Station自動接続フラグ
};
