#include <M5Unified.h>
#include "JsonCommandProcessor.h"

JsonCommandProcessor::JsonCommandProcessor() {}

void JsonCommandProcessor::begin(ParameterManager* pm, ResponseCallback callback) {
  parameterManager = pm;
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

void JsonCommandProcessor::handleUnknownCommand(const String& command) {
  StaticJsonDocument<128> response;
  response["error"] = "Unknown command";
  response["command"] = command;

  String out;
  serializeJson(response, out);
  responseCallback(out);
}
