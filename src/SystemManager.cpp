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
  if(index == 33){ timeZoneAreaId = newValue;}  // Pr.33: SNTP設定：タイムゾーンエリアID
  if(index == 34){ timeZoneId = newValue;}      // Pr.34: SNTP設定：タイムゾーンID
  if(index == 35){ timeZoneData = newValue;}    // Pr.35: SNTP設定：タイムゾーン

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
    {"timeZoneAreaId", 33},
    {"timeZoneId", 34},
    {"timeZone", 35},
    {"staAutoConnect", 44},
    // 必要に応じて追加
  };

  auto it = keyToParam.find(key);
  if (it != keyToParam.end()) {
    return parameterManager->setParameter(it->second, value); // 既存のsetParameter(番号, 値)を呼ぶ
  }
  return false; // 未対応のキー
}

bool SystemManager::setTimezone(uint8_t zoneData) {
  const char* gmt[] = {
  "GMT+12:00","GMT+11:00","GMT+10:00","GMT+09:30",
  "GMT+09:00","GMT+08:00","GMT+07:00","GMT+06:00",
  "GMT+05:00","GMT+04:00","GMT+03:30","GMT+03:00",
  "GMT+02:00","GMT+01:00","GMT-00:00","GMT-01:00",
  "GMT-02:00","GMT-03:00","GMT-03:30","GMT-04:00",
  "GMT-04:30","GMT-05:00","GMT-05:30","GMT-05:45",
  "GMT-06:00","GMT-06:30","GMT-07:00","GMT-08:00",
  "GMT-08:30","GMT-08:45","GMT-09:00","GMT-09:30",
  "GMT-10:00","GMT-10:30","GMT-11:00","GMT-12:00",
  "GMT-12:45","GMT-13:00","GMT-14:00"
  };

  if (zoneData < sizeof(gmt) / sizeof(gmt[0])) {
    std::cout << "Setting timezone to: " << gmt[zoneData] << "\n";

    const char* tz = gmt[zoneData];
    const std::string& tzStr = std::string(tz);
    timeManager->updateTimeZone(tzStr); // TimeManagerのupdateTimeZoneを呼び出す
    return true;
  }
  std::cout << "Invalid timezone data: " << static_cast<int>(zoneData) << "\n";
  return false;
}

std::string SystemManager::makeSettingJs(void) {
  std::string js = "var _initial_setting_ = \'{\\\n";
  
  js += "\"ntpSet\" : \"" + std::string(ntpSet ? "1" : "0") + "\",\\\n";
  js += "\"timeZoneAreaId\" : \"" + std::to_string(timeZoneAreaId) + "\",\\\n";
  js += "\"timeZoneId\" : \"" + std::to_string(timeZoneId) + "\",\\\n";

//  js += "  timeZone: " + std::to_string(timeZoneData) + "\",\\\n";
//  js += "  format12h: " + std::string(format12h ? "true" : "false") + ",\n";
//  js += "  display_format: " + std::to_string(display_format) + ",\n";
//  js += "  ntp_enable: " + std::string(ntpSet ? "true" : "false") + ",\n";
//  js += "  timeZoneAreaId: " + std::to_string(timeZoneAreaId) + ",\n";
//  js += "  timeZoneId: " + std::to_string(timeZoneId) + ",\n";
//  js += "  timeZone: " + std::to_string(timeZoneData) + ",\n";
//  js += "  staAutoConnect: " + std::string(staAutoConnect ? "true" : "false") + "\n";

  js += "\"end\" : \"\"\\\n";
  js += "}\';";
  return js;
}
