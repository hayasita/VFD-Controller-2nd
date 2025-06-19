#include "EnvironmentSensor.h"

EnvironmentSensor::EnvironmentSensor(I2CBusManager* busManager)
 : i2cBus(busManager)
{}

void EnvironmentSensor::begin(void) {
  return;
}

bool EnvironmentSensor::readSensorData() {
    std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());
    temperature = 25.0;
    humidity = 50.0;
    return true;
}

float EnvironmentSensor::getTemperature() const { return temperature; }
float EnvironmentSensor::getHumidity() const { return humidity; }
