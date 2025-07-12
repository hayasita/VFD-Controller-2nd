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
    const char* key;
    int paramNum;
  };

  static const KeyParamMap keyParamTable[] = {
    {"formatHour",        0},
    {"dispFormat",        1},
    {"timeDisplayFormat", 2},
    {"dateDisplayFormat", 3},
    {"displayEffect",     4},
    {"fadeTime",          5},
    {"glowInTheBrightSet",6},
    {"glowInTheDarkSet",  7},
    {"br_dig0",           8},
    {"br_dig1",           9},
    {"br_dig2",           10},
    {"br_dig3",           11},
    {"br_dig4",           12},
    {"br_dig5",           13},
    {"br_dig6",           14},
    {"br_dig7",           15},
    {"br_dig8",           16},
    {"br_dig9",           17},

    {"ntpSet",            32},
    {"timeZoneAreaId",    33},
    {"timeZoneId",        34},
    {"timeZone",          35},

    {"localesId",         43},
    {"staAutoConnect",    44},
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
