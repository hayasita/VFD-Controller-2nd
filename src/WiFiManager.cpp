#include "WiFiManager.h"

bool WiFiManager::connect(const char* ssid, const char* password, uint32_t timeout_ms) {
  std::lock_guard<std::mutex> lock(mutex);

  if (WiFi.status() == WL_CONNECTED) {
    return true;  // すでに接続されている
  }

  Serial.println("WiFi.begin");
  WiFi.begin(ssid, password);
  Serial.println("WiFi.begin_end");

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeout_ms) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    wasConnected = true;
    if (connectedCallback) connectedCallback();
    return true;
  } else {
    Serial.println("");
    Serial.println("WiFi connection failed.");
    wasConnected = false;
    return false;
  }
}

bool WiFiManager::isConnected() const {
  std::lock_guard<std::mutex> lock(mutex);
  return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::disconnect() {
  std::lock_guard<std::mutex> lock(mutex);
  WiFi.disconnect();
  wasConnected = false;
  if (disconnectedCallback) disconnectedCallback();
}

void WiFiManager::update() {
  std::lock_guard<std::mutex> lock(mutex);
  bool nowConnected = WiFi.status() == WL_CONNECTED;
  if (nowConnected && !wasConnected) {
    wasConnected = true;
    if (connectedCallback) connectedCallback();
  } else if (!nowConnected && wasConnected) {
    wasConnected = false;
    if (disconnectedCallback) disconnectedCallback();
  }
}

void WiFiManager::onConnected(std::function<void()> callback) {
  connectedCallback = callback;
}

void WiFiManager::onDisconnected(std::function<void()> callback) {
  disconnectedCallback = callback;
}
