#include <M5Unified.h>
#include "JsonCommandProcessor.h"

JsonCommandProcessor::JsonCommandProcessor(ParameterManager* pm, WiFiManager* wifiManager)
  : parameterManager(pm), wifiManager(wifiManager)
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
  jsondata = doc["getWifiStaList"]; // "getWifiStaList" キーを取得
  if (!jsondata.isNull()) {
    if(jsondata.as<int>() == 1){
      Serial.println("getWifiStaList1");
      handleGetWifiStaListCommand(doc);
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
    handleUnknownCommand(command);
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
