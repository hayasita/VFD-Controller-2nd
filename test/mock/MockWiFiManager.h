#pragma once
#include "../../src/WiFiManager.h"
#include <gmock/gmock.h>

class MockWiFiManager : public WiFiManager {
public:
    MockWiFiManager() : WiFiManager(nullptr) {} // ★追加（テスト用ダミー引数）

    MOCK_METHOD(void, update, (), (override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
    MOCK_METHOD(void, disconnect, (), (override));
    MOCK_METHOD(void, onConnected, (std::function<void()>), (override));
    MOCK_METHOD(void, onDisconnected, (std::function<void()>), (override));
    MOCK_METHOD(void, wifiScanRequest, (), (override));
    MOCK_METHOD(bool, wifiScanResult, (), (override));
    MOCK_METHOD(std::string, getWiFiScanResultString, (), (override));
    MOCK_METHOD(std::string, getWiFiScanResultJson, (), (override));
    MOCK_METHOD(void, setWifiScanCallback, (std::function<void()>), (override));
    MOCK_METHOD(bool, checkWifiScanCallback, (), (const, override));
    // 必要に応じて他のメソッドも追加
};
