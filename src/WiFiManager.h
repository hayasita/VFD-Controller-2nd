#pragma once

#include <WiFi.h>
#include <mutex>

class WiFiManager {
public:
  bool connect(const char* ssid, const char* password, uint32_t timeout_ms = 10000);
  void update(); // 状態確認用
  bool isConnected() const;
  void disconnect();
  void onConnected(std::function<void()> callback);
  void onDisconnected(std::function<void()> callback);

private:
  mutable std::mutex mutex;
  std::function<void()> connectedCallback;
  std::function<void()> disconnectedCallback;
  bool wasConnected = false;
};
