#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "WiFiManager.h"
#include "JsonCommandProcessor.h"
#include "ParameterManager.h"

class WebServerManager {
  public:
    WebServerManager(ParameterManager* parameterManager, JsonCommandProcessor* commandProcessor, WiFiManager* wifiManager);
    void begin();
    void end();
    void update();
    void setupRoutes();
    bool isRunning();

    bool hasWebSocketClients();  // ← WebSocketの接続状態を確認

    // 必要があればこちらも
    size_t getWebSocketClientCount();

  private:
    AsyncWebServer server;
    AsyncWebSocket ws;
    AsyncWebSocketClient* lastClient = nullptr;
    JsonCommandProcessor* jsonCommandProcessor = nullptr;
    ParameterManager* parameterManager = nullptr;
    WiFiManager* wifiManager = nullptr;
    bool running = false;
  
    void handleSettingjs(AsyncWebServerRequest *request);
    void handleNotFound(AsyncWebServerRequest *request);
    void onWebSocketEvent(AsyncWebSocket *server,
                          AsyncWebSocketClient *client,
                          AwsEventType type, void *arg,
                          uint8_t *data, size_t len);
};
  