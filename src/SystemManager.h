#pragma once

#include "SystemEvent.h"
#include "WiFiManager.h"
#include "TimeManager.h"

enum class SystemMode {
  Clock,
  WiFiConnected,
  WiFiDisconnected,
  // 他のモード
};

class SystemManager {
public:
  void begin(WiFiManager& wifi, TimeManager& time);
  void update(SystemEvent event);

  // パラメータ変更通知
  void onParameterChanged(uint8_t index, uint8_t newValue);

private:
  SystemMode currentMode = SystemMode::Clock;
  SystemMode currentWifiMode = SystemMode::WiFiDisconnected;
  WiFiManager* wifiManager = nullptr;
  TimeManager* timeManager = nullptr;

  bool format12h = false;       // 12時間表示フラグ Pr.1と連動

  bool ntpSet = false;          // NTP設定フラグ  Pr.30と連動
  bool staAutoConnect = false;  // WiFi Station自動接続フラグ Pr.44と連動

  void updateWiFiAutoConnect(void);

  bool isFormat12h() const { return format12h; }            // 12時間表示フォーマットフラグ
  bool isNtpAutoSet() const { return ntpSet; }              // NTP自動設定フラグ
  bool isNtpSet() const { return ntpSet; }                  // NTP設定フラグ
  bool isStaAutoConnect() const { return staAutoConnect; }  // WiFi Station自動接続フラグ
};
