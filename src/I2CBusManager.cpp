#include <M5Unified.h>
#include "I2CBusManager.h"
#include "Config.h"

void I2CBusManager::begin() {
  wire.begin(SDA_PIN, SCL_PIN, I2C_FREQ);
}

std::recursive_mutex& I2CBusManager::getMutex() {
  return i2cMutex;
}

TwoWire& I2CBusManager::getWire() {
  return wire;
}

/*
std::vector<uint8_t> I2CBusManager::scanI2CBus(uint8_t address, uint8_t count) {
  std::vector<uint8_t> foundDevices;
  std::lock_guard<std::recursive_mutex> lock(i2cMutex);
  for (uint8_t i = address; i <= count; i++) {
    if (wire.requestFrom(i, 1) > 0) {
      foundDevices.push_back(i);
    }
  }
  return foundDevices;
}
*/

std::vector<uint8_t> I2CBusManager::scanI2CBus(uint8_t startAddress, uint8_t count) {
//void DeviceChk::i2cScan(void){
//  std::vector<uint8_t> foundDevices;
  std::vector<uint8_t> i2cDevice;

  byte error, address;
  int nDevices;

//  i2cDevice.clear();    // DeviceList 初期化


//  Serial.println("Scanning...");
 
  nDevices = 0;
//  address = 0x50;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
//    Serial.println(error);

    if (error == 0)
    {
      i2cDevice.push_back(address);   // DeviceList 追加
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
    else{
//      Serial.println(address,HEX);
//      Serial.println(error);
    }
  }
//  if (nDevices == 0)
//    Serial.println("No I2C devices found.");
//  else
//    Serial.println("done.");
/*
  for (int i = 0; i < i2cDevice.size(); i++) {
    Serial.print("I2C device found at address 0x");
    if (i2cDevice[i]<16)
      Serial.print("0");
    Serial.print(i2cDevice[i],HEX);
    Serial.println("  !");
  }
*/

  return i2cDevice;
}

