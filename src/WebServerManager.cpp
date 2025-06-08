#include "WebServerManager.h"
#include <LittleFS.h>  // SPIFFSの代替。LittleFSを使う場合。

WebServerManager::WebServerManager(ParameterManager* param, JsonCommandProcessor* json, WiFiManager* wifi)
  : parameterManager(param),      // パラメータ管理クラスのインスタンスを設定
    jsonCommandProcessor(json),   // JSONコマンドプロセッサのインスタンスを設定
    wifiManager(wifi),            // WiFiManagerのインスタンスを設定
    server(80),                   // ポート80でAsyncWebServerを初期化
    ws("/ws")                     // WebSocketのルートを設定
{

  return;
}
// サーバー起動処理
//void WebServerManager::begin(ParameterManager* pm, JsonCommandProcessor* jcp, WiFiManager* wifiManager) {
void WebServerManager::begin() {
  if (running) return;  // すでに開始していればスキップ

//  parameterManager = pm;
//  jsonCommandProcessor = jcp;
  
  // LittleFS のマウント確認
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  // 各ルートパスの設定
  setupRoutes();

  // WebSocket イベントハンドラ登録
  ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client,
                    AwsEventType type, void *arg, uint8_t *data, size_t len) {
    this->onWebSocketEvent(server, client, type, arg, data, len);
  });

  // WebSocket ハンドラを Web サーバーに追加
  server.addHandler(&ws);

    // JSONコマンドプロセッサ初期化
  jsonCommandProcessor->begin([this](const String& response) {
    // WebSocketクライアントに送信
    if (lastClient && lastClient->canSend()) {
      lastClient->text(response);
    }
  });

  // サーバー起動
  server.begin();

  running = true;
}

// HTTPリクエストルートの設定
void WebServerManager::setupRoutes() {
  // ルートパス ("/") にアクセスされた場合の処理
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", String(), false);
  });
/*
  server.on("/setting.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleSettingjs(request);
  });
*/
  // ルート未登録のURLへのアクセス処理（静的ファイル配信）
  server.onNotFound([this](AsyncWebServerRequest *request) {
    handleNotFound(request);
  });
}

// ファイルパスに応じたContent-Type（MIME）を返す
String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".css"))  return "text/css";
  if (filename.endsWith(".js"))   return "application/javascript";
  if (filename.endsWith(".json")) return "application/json";
  if (filename.endsWith(".png"))  return "image/png";
  if (filename.endsWith(".jpg"))  return "image/jpeg";
  return "text/plain";  // 既知以外はテキスト
}

void WebServerManager::handleSettingjs(AsyncWebServerRequest *request) {
//  String html_tmp = makeSettingjs();

//  request->send(200, "application/javascript", html_tmp);

  return;
}
  
// 存在しないルートへアクセスされた時の処理
void WebServerManager::handleNotFound(AsyncWebServerRequest *request) {
  String path = request->url();

  // ディレクトリパスなら index.html にリダイレクト
  if (path.endsWith("/")) path += "index.html";

  if (LittleFS.exists(path)) {
    request->send(LittleFS, path, getContentType(path));
  } else {
    request->send(404, "text/plain", "Not Found");
  }
}

// 必要に応じて WebSocket クライアントの管理を行う処理（未使用だが拡張可）
void WebServerManager::update() {
  // WebSocketのpingなどをここで処理できる
  static unsigned long lastPing = 0;
  if (millis() - lastPing > 10000) {
//    Serial.println("Sending ping to all WebSocket clients");
    ws.pingAll();
    lastPing = millis();
  }

  return;
}

// WebSocket のイベントを処理する関数
void WebServerManager::onWebSocketEvent(AsyncWebSocket *server,
                                        AsyncWebSocketClient *client,
                                        AwsEventType type, void *arg,
                                        uint8_t *data, size_t len) {
  // テキストデータを受信したときのみ処理
  if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    // フレームが最終かつ最初（分割されていない）テキストデータである場合
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      // 受信データを文字列化
      String jsonString = String((char*)data, len);

      // クライアントを保持（応答の送信に使う）
      lastClient = client;

      // JsonCommandProcessor にコマンド処理を委譲
      jsonCommandProcessor->processCommand(jsonString);
    }
  } else if (type == WS_EVT_CONNECT) {
    Serial.printf("[WS] Client connected: %u\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("[WS] Client disconnected: %u\n", client->id());
    if (client == lastClient) {
      lastClient = nullptr;  // 応答先が切断されたらクリア
    }
  }

}

void WebServerManager::end() {
  if (!running) return;  // 動作していなければスキップ
  // WebSocket クライアントのすべての接続を閉じる
  ws.closeAll(); // WebSocket通信を切断

  // Webサーバを停止
  server.end();

  // LittleFSのアンマウント（必要であれば）
  // LittleFS.end(); // 通常は不要

  running = false;
}

bool WebServerManager::isRunning() {
  return running;
}

bool WebServerManager::hasWebSocketClients() {
  return ws.count() > 0;
}

size_t WebServerManager::getWebSocketClientCount() {
  return ws.count();
}
