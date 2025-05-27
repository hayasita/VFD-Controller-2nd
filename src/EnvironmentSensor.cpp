#include "EnvironmentSensor.h"

void EnvironmentSensor::begin(I2CBusManager& i2cBus) {
    this->i2cBus = &i2cBus;
}

bool EnvironmentSensor::readSensorData() {
    std::lock_guard<std::recursive_mutex> lock(i2cBus->getMutex());
    temperature = 25.0;
    humidity = 50.0;
    return true;
}

float EnvironmentSensor::getTemperature() const { return temperature; }
float EnvironmentSensor::getHumidity() const { return humidity; }
