#pragma once
#include <vector>
#include <mutex>
#include <Wire.h>

class DummyI2CBusManager : public I2CBusManager {
  public:
    void begin() override {}
    std::recursive_mutex& getMutex() override { return dummyMutex; }
    TwoWire& getWire() override { return wire; }
    std::vector<uint8_t> scanI2CBus(uint8_t address = 0x00, uint8_t count = 127) override {
      return {}; // モックなので空のリストを返す
    }
  private:
    std::recursive_mutex dummyMutex;
    TwoWire wire = TwoWire(0);  // 0番ポートを使用（ESP32）
};
