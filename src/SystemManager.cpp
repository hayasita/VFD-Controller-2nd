#include <iostream>
#include "SystemManager.h"

void SystemManager::begin(WiFiManager& wifi, TimeManager& time) {
  wifiManager = &wifi;
  timeManager = &time;
}

void SystemManager::update(SystemEvent event) {
  switch (event) {
    case SystemEvent::ButtonA_Short_Pressed:
      if(currentWifiMode == SystemMode::WiFiDisconnected) {
        currentWifiMode = SystemMode::WiFiConnected;
//        wifiManager->connect("", "");
        wifiManager->withItm(); // 端子入力でWiFi接続要求
        std::cout << "WiFi connection requested via terminal input.\n";
/*        if (wifiManager->isConnected()) {
          Serial.println("WiFi is connected.");
//          timeManager->syncWithSNTP("ntp.nict.jp", 9 * 3600);  // JST (UTC+9)
        } else {
          Serial.println("Failed to connect to WiFi.");
        }
*/
        } else {
        currentWifiMode = SystemMode::WiFiDisconnected;
//        wifiManager->disconnect();
        wifiManager->withItm(); // 端子入力でWiFi接続要求
        std::cout << "WiFi disconnected.\n";
      }
//      currentMode = SystemMode::Clock;
      break;

    case SystemEvent::WebCommand_ConnectWiFi:
//            wifiManager->connect();
      currentMode = SystemMode::WiFiConnected;
      break;

    case SystemEvent::WebCommand_DisconnectWiFi:
//            wifiManager->disconnect();
      currentMode = SystemMode::WiFiDisconnected;
      break;

    case SystemEvent::Scheduled_SyncTime:
//            timeManager->syncWithSNTP();
      break;

    default:
      break;
  }
}

/**
 *  @brief パラメータ変更通知
 *  @param index パラメータのインデックス
 *  @param newValue 新しい値
 *  この関数は、パラメータの値が変更されたときに呼び出され、動作フラグを更新する。
 */
void SystemManager::onParameterChanged(uint8_t index, uint8_t newValue) {
  std::cout << "SystemManager::onParameterChanged: index=" << static_cast<int>(index) << ", newValue=" << static_cast<int>(newValue) << "\n";

  if(index == 0) format12h = (bool)newValue; // Pr.0: 12時間表示フォーマット

  if(index == 30) ntpSet = (bool)newValue;
  if(index == 44) staAutoConnect = (bool)newValue;

  updateWiFiAutoConnect();

  return;
}

/**
 * @brief WiFi自動接続設定の更新
 * この関数は、NTP設定とWiFi Station自動接続フラグに基づいてWiFiManagerの自動接続設定を更新する。
 * NTPが設定されていて、WiFi Station自動接続が有効な場合は自動接続を有効にし、
 * それ以外の場合は無効にする。
 */
void SystemManager::updateWiFiAutoConnect(void) {
  if(ntpSet && staAutoConnect) {
    wifiManager->setAutoConnect(true); // WiFiManagerに自作のON/OFFメソッドを用意
    std::cout << "WiFi起動時自動接続: ON\n";
  }
  else {
    wifiManager->setAutoConnect(false);
    std::cout << "WiFi起動時自動接続: OFF\n";
  }

  return;
}

