#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "WiFiManager.h"
#include "JsonCommandProcessor.h"
#include "ParameterManager.h"
#include "SystemManager.h"  // システム管理クラス

class WebServerManager {
  public:
    WebServerManager(ParameterManager* parameterManager, JsonCommandProcessor* commandProcessor, WiFiManager* wifiManager);
    void begin();
    void end();
    void update();
    void setupRoutes();
    bool isRunning();

    bool hasWebSocketClients();  // ← WebSocketの接続状態を確認

    void onMakeSettingJs(std::function<std::string()> callback);  // setting.js 生成コールバックを設定

    // 必要があればこちらも
    size_t getWebSocketClientCount();

  private:
    AsyncWebServer server;
    AsyncWebSocket ws;
    AsyncWebSocketClient* lastClient = nullptr;
    JsonCommandProcessor* jsonCommandProcessor = nullptr;
    ParameterManager* parameterManager = nullptr;
    SystemManager* systemManager = nullptr;                 // システム管理クラスへのポインタ
    WiFiManager* wifiManager = nullptr;
    bool running = false;
  
    void handleSettingjs(AsyncWebServerRequest *request);
    void handleNotFound(AsyncWebServerRequest *request);
    void onWebSocketEvent(AsyncWebSocket *server,
                          AsyncWebSocketClient *client,
                          AwsEventType type, void *arg,
                          uint8_t *data, size_t len);

    std::function<std::string()> makeSettingJsCallback;     // setting.js 生成コールバック
};
  