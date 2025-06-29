#pragma once

#include "../../src/SystemManager.h"

class DummySystemManager : public SystemManager {
public:
  void begin(WiFiManager& wifi, TimeManager& time, ParameterManager& parameter) override {
    // モックの初期化処理
  }
  void update(SystemEvent event) override {
    // モックの更新処理
  }
  void onParameterChanged(uint8_t index, uint8_t newValue) override {
  }
  bool setParameterByKey(const std::string& key, int value) override {
    // モックのパラメータ設定処理
    return true;  // 成功を返す
  }
  bool setTimezone(uint8_t zoneData) override {
    // モックのタイムゾーン設定処理
    return true;  // 成功を返す
  }
  void updateWiFiAutoConnect(void) override {
    // モックのWiFi自動接続更新処理
  }
  std::string makeSettingJs(void) override { return "{}"; }
};
