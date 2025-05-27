#pragma once

#include "SystemEvent.h"
#include "WiFiManager.h"
#include "TimeManager.h"

enum class SystemMode {
    Clock,
    WiFiConnected,
    WiFiDisconnected,
    // 他のモード
};

class SystemManager {
public:
    void begin(WiFiManager& wifi, TimeManager& time);
    void update(SystemEvent event);

private:
    SystemMode currentMode = SystemMode::Clock;
    SystemMode currentWifiMode = SystemMode::WiFiDisconnected;
    WiFiManager* wifiManager = nullptr;
    TimeManager* timeManager = nullptr;
};
