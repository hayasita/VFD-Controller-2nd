#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "WiFiManager.h"
#include "JsonCommandProcessor.h"
#include "ParameterManager.h"

class WebServerManager {
  public:
    void begin(ParameterManager* parameterManager, JsonCommandProcessor* commandProcessor);
    void end();
    void update();
    void setupRoutes();
    void setWiFiManager(WiFiManager* wifi);
    bool isRunning();

    bool hasWebSocketClients();  // ← WebSocketの接続状態を確認

    // 必要があればこちらも
    size_t getWebSocketClientCount();

  private:
    AsyncWebServer server{80};
    AsyncWebSocket ws{"/ws"};
    AsyncWebSocketClient* lastClient = nullptr;
    JsonCommandProcessor* commandProcessor = nullptr;
    ParameterManager* parameterManager = nullptr;
    WiFiManager* wifiManager = nullptr;
    bool running = false;
  
    void handleSettingjs(AsyncWebServerRequest *request);
    void handleNotFound(AsyncWebServerRequest *request);
    void onWebSocketEvent(AsyncWebSocket *server,
                          AsyncWebSocketClient *client,
                          AwsEventType type, void *arg,
                          uint8_t *data, size_t len);
    void handleCommand(AsyncWebSocketClient* client, JsonDocument& doc);
};
  