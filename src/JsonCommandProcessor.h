#pragma once

#include <ArduinoJson.h>
#include <functional>
#include "ParameterManager.h"

/**
 * @brief JSONコマンド処理クラス
 * - 受信したJSON文字列を解析して適切なアクションを実行
 * - WebSocket、シリアルなど様々な通信手段からの入力に対応可能
 * - ParameterManager との連携で、パラメータの読み書きを行う
 */
class JsonCommandProcessor {
public:
  // 応答データ送信関数（WebSocketやシリアル出力に利用される）
  using ResponseCallback = std::function<void(const String&)>;

  JsonCommandProcessor();

  // パラメータ管理と応答用コールバックをセット
  void begin(ParameterManager* parameterManager, ResponseCallback callback);

  // JSONコマンド文字列の処理
  void processCommand(const String& jsonString);

private:
  ParameterManager* parameterManager = nullptr;
  ResponseCallback responseCallback;

  // 内部コマンド処理（個別に関数化）
  void handlePingCommand(JsonDocument& doc);
  void handleGetCommand(JsonDocument& doc);
  void handleSetCommand(JsonDocument& doc);
  void handleUnknownCommand(const String& command);
};
