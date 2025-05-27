#pragma once
#include <mutex>
#include <vector>

// ダミーTwoWire
class TwoWire {
public:
    TwoWire(int) {}
};

class I2CBusManager {
public:
    virtual void begin();
    virtual std::recursive_mutex& getMutex();
    virtual TwoWire& getWire();
    virtual std::vector<uint8_t> scanI2CBus(uint8_t address = 0x00, uint8_t count = 127);
};