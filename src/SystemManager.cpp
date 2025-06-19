#include <iostream>
#include <map>
#include "SystemManager.h"
#include "parameterManager.h"

void SystemManager::begin(WiFiManager& wifi, TimeManager& time, ParameterManager& parameter) {
  wifiManager = &wifi;
  timeManager = &time;
  parameterManager = &parameter;
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
  std::cout << "ntpSet : " << ntpSet << "\n";
  std::cout << "staAutoConnect : " << staAutoConnect << "\n";
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

bool SystemManager::setParameterByKey(const std::string& key, int value) {
  // キー名とパラメータ番号の対応表
  static const std::map<std::string, uint8_t> keyToParam = {
  //  {"staAutoConnect",0},
    {"ntp_enable", 30},
    {"staAutoConnect", 44},
    // 必要に応じて追加
  };

  auto it = keyToParam.find(key);
  if (it != keyToParam.end()) {
    return parameterManager->setParameter(it->second, value); // 既存のsetParameter(番号, 値)を呼ぶ
  }
  return false; // 未対応のキー
}
