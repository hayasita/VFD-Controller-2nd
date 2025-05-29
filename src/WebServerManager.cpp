#include "WebServerManager.h"
#include <LittleFS.h>  // SPIFFSの代替。LittleFSを使う場合。

// サーバー起動処理
void WebServerManager::begin(ParameterManager* pm, JsonCommandProcessor* cp) {
  if (running) return;  // すでに開始していればスキップ

  parameterManager = pm;
  commandProcessor = cp;
  
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
  commandProcessor->begin(parameterManager, [this](const String& response) {
    // WebSocketクライアントに送信
    if (lastClient && lastClient->canSend()) {
      lastClient->text(response);
    }
  });

  // サーバー起動
  server.begin();

  running = true;
}

// 外部から WiFiManager のインスタンスを設定
void WebServerManager::setWiFiManager(WiFiManager* wifi) {
  this->wifiManager = wifi;
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
      commandProcessor->processCommand(jsonString);
    }
  } else if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket client disconnected");
    if (client == lastClient) {
      lastClient = nullptr;  // 応答先が切断されたらクリア
    }
  }

}

// 受け取った JSON コマンドを処理する関数
void WebServerManager::handleCommand(AsyncWebSocketClient* client, JsonDocument& doc) {
  // "cmd" キーが存在しない場合はエラー応答
  if (!doc.containsKey("cmd")) {
    client->text("{\"error\":\"Missing command\"}");
    return;
  }

  String cmd = doc["cmd"].as<String>();

  // WiFi 再接続要求コマンド
  if (cmd == "wifiReconnect") {
    if (wifiManager) {
      // ここでは仮SSID/PASS使用（実際には引数として受け取る方がよい）
//      bool result = wifiManager->connect("your-ssid", "your-password");
//      client->text(String("{\"status\":\"") + (result ? "connected" : "failed") + "\"}");
    } else {
      client->text("{\"error\":\"WiFiManager not available\"}");
    }
  }
  // WebSocketの接続確認
  else if (cmd == "ping") {
    client->text("{\"response\":\"pong\"}");
  }
  // 未定義コマンド
  else {
    client->text("{\"error\":\"Unknown command\"}");
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
