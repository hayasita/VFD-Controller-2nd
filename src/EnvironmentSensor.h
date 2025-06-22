#pragma once

#include "I2CBusManager.h"

/**
 * @brief 環境センサ管理クラス
 */
class EnvironmentSensor {
public:
  EnvironmentSensor(I2CBusManager* busManager);
  void begin(void);
  bool readSensorData();
  float getTemperature() const;
  float getHumidity() const;

private:
  I2CBusManager* i2cBus = nullptr;
  float temperature = 0.0;
  float humidity = 0.0;
};
