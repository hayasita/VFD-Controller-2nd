#include <iostream>
#include <map>
#include "SystemManager.h"
#include "parameterManager.h"

/**
 * @brief 依存関係の初期化
 * @param wifi WiFiManagerの参照
 * @param time TimeManagerの参照
 * @param parameter ParameterManagerの参照
 * @param terminal TerminalInputManagerの参照
 * @param builtInLed BuiltInLedControllerの参照
 * @param externalLed ExternalLedControllerの参照
 */
void SystemManager::initDependencies(WiFiManager& wifi, TimeManager& time, ParameterManager& parameter, TerminalInputManager& terminal, LedController& builtInLed, LedController& externalLed)
{
  wifiManager = &wifi;
  timeManager = &time;
  parameterManager = &parameter;
  terminalInputManager = &terminal;
  builtInLedCtrl = &builtInLed;
  externalLedCtrl = &externalLed;
  return;
}

/**
 * @brief システム起動処理
 *  パラメータ初期化後のシステムマネージャの初期化を行う。
*/
void SystemManager::begin(void)    // 初期化処理
{
  // 初期化処理の実装
  std::cout << "SystemManager initialized.\n";

  // LEDの初期設定
  builtInLedCtrl->setMode(0, LedMode::Blink, CRGB::Green);
  externalLedCtrl->setMode(0, LedMode::On, CRGB::Green);
  externalLedCtrl->setMode(1, LedMode::Blink, CRGB::Blue);
  externalLedCtrl->setMode(2, LedMode::On, CRGB::Orange);

  wifiManager->withBoot();  // ブート時のWiFi接続要求

  return;
}

void SystemManager::update(void) {

  SystemEvent event = terminalInputManager->update();
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

  if(index == static_cast<uint8_t>(ParamIndex::Format12h)){ format12h = (bool)newValue;}              // Pr.0: 12時間表示フォーマット
  if(index == static_cast<uint8_t>(ParamIndex::DispFormat)){ dispFormat = newValue;}                  // Pr.1: 表示フォーマット
  if(index == static_cast<uint8_t>(ParamIndex::TimeDisplayFormat)){ timeDisplayFormat = newValue;}    // Pr.2: 時刻表示フォーマット
  if(index == static_cast<uint8_t>(ParamIndex::DateDisplayFormat)){ dateDisplayFormat = newValue;}    // Pr.3: 日付表示フォーマット
  if(index == static_cast<uint8_t>(ParamIndex::DisplayEffect)){ displayEffect = newValue;}            // Pr.4: 表示効果
  if(index == static_cast<uint8_t>(ParamIndex::FadeTime)){ fadetimew = newValue;}                     // Pr.5
  if(index == static_cast<uint8_t>(ParamIndex::GlowInTheBrightTmp)){ glowInTheBrightTmp = newValue;}  // Pr.6: 全体輝度設定値：明
  if(index == static_cast<uint8_t>(ParamIndex::GlowInTheDarkTmp)){ glowInTheDarkTmp = newValue;}      // Pr.7: 全体輝度設定値：暗
  if(index == static_cast<uint8_t>(ParamIndex::BrDig0)){ brDig[0] = newValue;}                        // Pr.8: 表示桁0の輝度
  if(index == static_cast<uint8_t>(ParamIndex::BrDig1)){ brDig[1] = newValue;}                        // Pr.9: 表示桁1の輝度
  if(index == static_cast<uint8_t>(ParamIndex::BrDig2)){ brDig[2] = newValue;}                        // Pr.10: 表示桁2の輝度
  if(index == static_cast<uint8_t>(ParamIndex::BrDig3)){ brDig[3] = newValue;}                        // Pr.11: 表示桁3の輝度
  if(index == static_cast<uint8_t>(ParamIndex::BrDig4)){ brDig[4] = newValue;}                        // Pr.12: 表示桁4の輝度
  if(index == static_cast<uint8_t>(ParamIndex::BrDig5)){ brDig[5] = newValue;}                        // Pr.13: 表示桁5の輝度
  if(index == static_cast<uint8_t>(ParamIndex::BrDig6)){ brDig[6] = newValue;}                        // Pr.14: 表示桁6の輝度
  if(index == static_cast<uint8_t>(ParamIndex::BrDig7)){ brDig[7] = newValue;}                        // Pr.15: 表示桁7の輝度
  if(index == static_cast<uint8_t>(ParamIndex::BrDig8)){ brDig[8] = newValue;}                        // Pr.16: 表示桁8の輝度

  if(index == static_cast<uint8_t>(ParamIndex::NtpSet)){ ntpSet = (bool)newValue;}                    // Pr.32: SNTP設定：SNTP使用
  if(index == static_cast<uint8_t>(ParamIndex::TimeZoneAreaId)){ timeZoneAreaId = newValue;}          // Pr.33: SNTP設定：タイムゾーンエリアID
  if(index == static_cast<uint8_t>(ParamIndex::TimeZoneId)){ timeZoneId = newValue;}                  // Pr.34: SNTP設定：タイムゾーンID
  if(index == static_cast<uint8_t>(ParamIndex::TimeZoneData)){ timeZoneData = newValue;}              // Pr.35: SNTP設定：タイムゾーン

  if(index == static_cast<uint8_t>(ParamIndex::LocalesId)){ localesId = newValue;}                    // Pr.43: 地域設定
  if(index == static_cast<uint8_t>(ParamIndex::StaAutoConnect)){ staAutoConnect = (bool)newValue;}    // Pr.44: WiFi Station 設定：STA自動接続有効

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

bool SystemManager::setParameterByPrnum(uint8_t prnum, int value) {
  return parameterManager->setParameter(prnum, value);
}
/*
bool SystemManager::setParameterByKey(const std::string& key, int value) {
  // キー名とパラメータ番号の対応表
  static const std::map<std::string, uint8_t> keyToParam = {
  //  {"staAutoConnect",0},
    {"format12h", 0},         // Pr.0: 12時間表示フォーマット
    {"display_format", 1},    // Pr.1: 表示フォーマット
    {"ntp_enable", 32},
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
*/
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

/**
 * @brief 輝度情報個別設定
 * @param adj_point 調整ポイント（桁数）
 * @param brw 輝度値
 * この関数は、指定された桁の輝度値を設定する。
 * パラメータの書き込みは実施しない。RAM値のみ更新する。
 */
void SystemManager::setBrDig(uint8_t adj_point,uint8_t brw) // 輝度情報個別設定
{
  if((brw >= BR_MIN) && (brw<=BR_MAX)&&(adj_point<DISP_KETAMAX)){
    brDig[adj_point] = brw;
  }

  return;
}

bool SystemManager::setParameterBrDig(void) {
  for (uint8_t i = 0; i < DISP_KETAMAX; ++i) {
    if (brDig[i] != 0) {
      // パラメータの書き込み処理を実施
      parameterManager->setParameter(8 + i, brDig[i]);
    }
  }
  return true;
}

bool SystemManager::resetBrDig(void) {
  for (uint8_t i = 0; i < DISP_KETAMAX; ++i) {
    brDig[i] = parameterManager->getParameter(8 + i); // 輝度値をリセット
  }
  return true;
}

std::string SystemManager::makeSettingJs(void) {
  std::string js = "var _initial_setting_ = \'{\\\n";
  
  js += "\"localesId\" : \"" + std::to_string(localesId) + "\",\\\n";
  
  js += "\"ntpSet\" : \"" + std::string(ntpSet ? "1" : "0") + "\",\\\n";
  js += "\"timeZoneAreaId\" : \"" + std::to_string(timeZoneAreaId) + "\",\\\n";
  js += "\"timeZoneId\" : \"" + std::to_string(timeZoneId) + "\",\\\n";

  js += "\"dispFormat\" : \"" + std::to_string(dispFormat) + "\",\\\n";
  js += "\"timeDisplayFormat\" : \"" + std::to_string(timeDisplayFormat) + "\",\\\n";
  js += "\"dateDisplayFormat\" : \"" + std::to_string(dateDisplayFormat) + "\",\\\n";
  js += "\"formatHour\" : \"" + std::string(format12h ? "1" : "0") + "\",\\\n";
  js += "\"displayEffect\" : \"" + std::to_string(displayEffect) + "\",\\\n";
  js += "\"fadeTime\" : \"" + std::to_string(fadetimew) + "\",\\\n";
  js += "\"brDig\" : [";
  for (size_t i = 0; i < DISP_KETAMAX; ++i) {
    js += std::to_string(brDig[i]);
    if (i < DISP_KETAMAX - 1) {
      js += ",";
    }
  }
  js += "],\\\n";
  js += "\"glowInTheBright\" : \"" + std::to_string(glowInTheBright) + "\",\\\n";
  js += "\"glowInTheDark\" : \"" + std::to_string(glowInTheDark) + "\",\\\n";

  js += "\"staAutoConnect\" : \"" + std::string(staAutoConnect ? "1" : "0") + "\",\\\n";

  js += "\"end\" : \"\"\\\n";
  js += "}\';";
  return js;
}
