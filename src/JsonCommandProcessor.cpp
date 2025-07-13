#include <M5Unified.h>
#include "JsonCommandProcessor.h"

JsonCommandProcessor::JsonCommandProcessor(ParameterManager* pm, WiFiManager* wifiManager, SystemManager* systemManager)
  : parameterManager(pm), wifiManager(wifiManager), systemManager(systemManager)
{
  return;
}

void JsonCommandProcessor::begin(ResponseCallback callback) {
  responseCallback = callback;
}

void JsonCommandProcessor::processCommand(const String& jsonString) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, jsonString);
  Serial.println("Received JSON: " + jsonString);

  if (error) {
    responseCallback("{\"error\":\"Invalid JSON\"}");
    return;
  }

  JsonVariant jsondata;
  // WiFi Station 設定：WiFi SSID 検索
  jsondata = doc["getWifiStaList"]; // "getWifiStaList" キーを取得
  if (!jsondata.isNull()) {
    if(jsondata.as<int>() == 1){
      Serial.println("getWifiStaList1");
      handleGetWifiStaListCommand(doc);
    }
  }

  struct KeyParamMap {
    const char* key;    // JSONのキー名
    uint8_t paramNum;   // 対応するパラメータ番号
  };

  static const KeyParamMap keyParamTable[] = {
    {"formatHour",        static_cast<uint8_t>(ParamIndex::Format12h)},         // 12時間表示フォーマット Pr.0
    {"dispFormat",        static_cast<uint8_t>(ParamIndex::DispFormat)},        // 表示フォーマット Pr.1
    {"timeDisplayFormat", static_cast<uint8_t>(ParamIndex::TimeDisplayFormat)}, // 時刻表示フォーマット Pr.2
    {"dateDisplayFormat", static_cast<uint8_t>(ParamIndex::DateDisplayFormat)}, // 日付表示フォーマット Pr.3
    {"displayEffect",     static_cast<uint8_t>(ParamIndex::DisplayEffect)},     // 表示効果 Pr.4
    {"fadeTime",          static_cast<uint8_t>(ParamIndex::FadeTime)},          // クロスフェード時間 Pr.5
    {"glowInTheBrightSet",static_cast<uint8_t>(ParamIndex::GlowInTheBrightTmp)},// 全体輝度設定値：明（テンポラリ） Pr.6
    {"glowInTheDarkSet",  static_cast<uint8_t>(ParamIndex::GlowInTheDarkTmp)},  // 全体輝度設定値：暗（テンポラリ） Pr.7
    {"glowInTheDarkSet",  static_cast<uint8_t>(ParamIndex::GlowInTheDarkTmp)},  // 全体輝度設定値：暗（テンポラリ） Pr.7
    {"br_dig0",           static_cast<uint8_t>(ParamIndex::BrDig0)},            // 表示桁0の輝度 Pr.8
    {"br_dig1",           static_cast<uint8_t>(ParamIndex::BrDig1)},            // 表示桁1の輝度 Pr.9
    {"br_dig2",           static_cast<uint8_t>(ParamIndex::BrDig2)},            // 表示桁2の輝度 Pr.10
    {"br_dig3",           static_cast<uint8_t>(ParamIndex::BrDig3)},            // 表示桁3の輝度 Pr.11
    {"br_dig4",           static_cast<uint8_t>(ParamIndex::BrDig4)},            // 表示桁4の輝度 Pr.12
    {"br_dig5",           static_cast<uint8_t>(ParamIndex::BrDig5)},            // 表示桁5の輝度 Pr.13
    {"br_dig6",           static_cast<uint8_t>(ParamIndex::BrDig6)},            // 表示桁6の輝度 Pr.14
    {"br_dig7",           static_cast<uint8_t>(ParamIndex::BrDig7)},            // 表示桁7の輝度 Pr.15
    {"br_dig8",           static_cast<uint8_t>(ParamIndex::BrDig8)},            // 表示桁8の輝度 Pr.16

    {"ntpSet",            static_cast<uint8_t>(ParamIndex::NtpSet)},            // SNTP設定：SNTP使用 Pr.32
    {"timeZoneAreaId",    static_cast<uint8_t>(ParamIndex::TimeZoneAreaId)},    // SNTP設定：タイムゾーンエリアID Pr.33
    {"timeZoneId",        static_cast<uint8_t>(ParamIndex::TimeZoneId)},        // SNTP設定：タイムゾーンID Pr.34
    {"timeZone",          static_cast<uint8_t>(ParamIndex::TimeZoneData)},      // SNTP設定：タイムゾーン Pr.35

    {"localesId",         static_cast<uint8_t>(ParamIndex::LocalesId)},         // 地域設定 Pr.43
    {"staAutoConnect",    static_cast<uint8_t>(ParamIndex::StaAutoConnect)},    // WiFi Station自動接続フラグ Pr.44
    // 必要に応じて追加
  };

  for (const auto& entry : keyParamTable) {
    JsonVariant jsondata = doc[entry.key];
    if (!jsondata.isNull()) {
      int value = jsondata.as<int>();
      Serial.println(String(entry.key) + " = " + String(value));
      bool success = systemManager->setParameterByPrnum(entry.paramNum, value);
      // 必要ならsuccessの利用やエラー処理も追加
    }
  }

  jsondata = doc["glowInTheBrighttmp"]; // "glowInTheBrighttmp" キーを取得
  if (!jsondata.isNull()) {
    uint8_t value = jsondata.as<uint8_t>();
    Serial.println("glowInTheBrighttmp = " + String(value));
    systemManager->onParameterChanged(6, value); // SystemManagerを経由して動作パラメータ設定する
  }
  jsondata = doc["glowInTheDarktmp"]; // "glowInTheDarktmp" キーを取得
  if (!jsondata.isNull()) {
    uint8_t value = jsondata.as<uint8_t>();
    Serial.println("glowInTheDarktmp = " + String(value));
    systemManager->onParameterChanged(7, value); // SystemManagerを経由して動作パラメータ設定する
  }

    // brDig
    jsondata = doc["brDig"];
    if (!jsondata.isNull()) {
      JsonArray array = jsondata.as<JsonArray>();
      Serial.print("array.size()=");
      Serial.println(array.size());
      for (uint8_t i = 0; i< array.size(); i++){
        uint8_t data = array[i];
        systemManager->setBrDig(i,data);
      }
    }

    // resetBrSetting 各桁の個別輝度設定 リセット処理
    jsondata = doc["resetBrSetting"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.println("[resetBrSetting]");
      if(num == 1){
        systemManager->resetBrDig();
      }
    }

    // writeBrSetting 各桁の個別輝度設定 保存処理
    jsondata = doc["writeBrSetting"];
    if (!jsondata.isNull()) {
      uint8_t num = jsondata.as<unsigned int>();
      Serial.println("[writeBrSetting]");
      if(num == 1){
        systemManager->setParameterBrDig();
      }
    }


  if (!doc.containsKey("command")) {
    responseCallback("{\"error\":\"Missing command field\"}");
    return;
  }

  String command = doc["command"].as<String>();
  Serial.println("Command: " + command);
  // コマンドに応じた処理を実行
  if (command == "get") {
    handleGetCommand(doc);
  } else if (command == "set") {
    handleSetCommand(doc);
  } else if (command == "ping") {
    handlePingCommand(doc);
  } else {
//    handleUnknownCommand(command);
  }
}

void JsonCommandProcessor::handlePingCommand(JsonDocument& doc) {
  DynamicJsonDocument response(128);
  response["status"] = "ok";
  response["response"] = "pong";

  String out;
  serializeJson(response, out);
  responseCallback(out);
}

void JsonCommandProcessor::handleGetCommand(JsonDocument& doc) {
  if (!doc.containsKey("index")) {
    responseCallback("{\"error\":\"Missing index for get\"}");
    return;
  }

  int index = doc["index"].as<int>();
  int value = parameterManager->getParameter(index);

  StaticJsonDocument<128> response;
  response["command"] = "get";
  response["index"] = index;
  response["value"] = value;

  String out;
  serializeJson(response, out);
  responseCallback(out);
}

void JsonCommandProcessor::handleSetCommand(JsonDocument& doc) {
  if (!doc.containsKey("index") || !doc.containsKey("value")) {
    responseCallback("{\"error\":\"Missing index or value for set\"}");
    return;
  }

  int index = doc["index"].as<int>();
  int value = doc["value"].as<int>();

  bool success = parameterManager->setParameter(index, value);

  StaticJsonDocument<128> response;
  response["command"] = "set";
  response["index"] = index;
  response["value"] = value;
  response["result"] = success ? "ok" : "fail";

  String out;
  serializeJson(response, out);
  responseCallback(out);
}


void JsonCommandProcessor::handleGetWifiStaListCommand(JsonDocument& doc) {
  // WiFi STAリストの取得処理を実装
  StaticJsonDocument<256> response;
  response["command"] = "getWifiStaList";
  response["staList"] = JsonArray(); // 空の配列を返す

  if(wifiManager && !(wifiManager->checkWifiScanCallback())) { // WiFiManagerが初期化されていて、スキャンコールバックが設定されていない場合
    Serial.println("Getting WiFi STA list...");
    // WiFiManagerのスキャンコールバックを設定
    wifiManager->setWifiScanCallback([this](/*std::string /*scanResult*/) {
      std::string result = wifiManager->getWiFiScanResultJson();
      Serial.println(result.c_str());
      responseCallback(result.c_str());
    });
    // WiFiManagerのスキャン機能を呼び出す
//    wifiManager->wifiScanRequest();     // WiFiスキャン要求を実行 ** 直接呼び出すと失敗する **
    wifiManager->wifiScanRequestFlag = true; // スキャンリクエストフラグを立てる
  }
  else if(wifiManager && wifiManager->checkWifiScanCallback()) { // スキャンコールバックが設定されている場合
    Serial.println("WiFiスキャンが既に実行中です。\n");
  }
  else {
    Serial.println("WiFiManagerが初期化されていません。\n");
  }

  String out;
  serializeJson(response, out);
  responseCallback(out);
}

void JsonCommandProcessor::handleUnknownCommand(const String& command) {
  StaticJsonDocument<128> response;
  response["error"] = "Unknown command";
  response["command"] = command;

  String out;
  serializeJson(response, out);
  responseCallback(out);
}
