//#include "../../src/I2CBusManager.h"
//#include "../mock/I2CBusManager.h"
#include "I2CBusManager.h"

void I2CBusManager::begin() {}
std::recursive_mutex& I2CBusManager::getMutex() { static std::recursive_mutex m; return m; }

TwoWire& I2CBusManager::getWire() { static TwoWire w(0); return w; }
std::vector<uint8_t> I2CBusManager::scanI2CBus(uint8_t, uint8_t) { return {}; }
