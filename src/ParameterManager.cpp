// ParameterManager.cpp
#include "ParameterManager.h"
#include <cstdio> // snprintf
#include <iostream>

#define MAX_PARAMS 50  // 最大パラメータ数

ParameterManager::ParameterManager(EepromManager *eeprom, LogManager *logger)
  : eeprom(eeprom),     // EepromManagerの参照を初期化
    storage(eeprom),    // パラメータストレージを初期化
    logger(logger)      // LogManagerの参照を初期化
{
  // コンストラクタで初期化は行わない
}

ParameterManager::~ParameterManager() {}

void ParameterManager::begin(void) {

  params.clear();
  params.reserve(MAX_PARAMS);  // パラメータ数を予約
  
  params.emplace_back(Parameter{0, 1, 0,  1, nullptr});   // Pr.0 初期化
  params.emplace_back(Parameter{0, 3, 1, 10, nullptr});   // Pr.1 初期化
  params.emplace_back(Parameter{0, 3, 1, 10, nullptr});   // Pr.2 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.3 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.4 初期化
  params.emplace_back(Parameter{0, 3, 1, 10, nullptr});   // Pr.5 初期化
  params.emplace_back(Parameter{0, 3, 1, 10, nullptr});   // Pr.6 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.7 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.8 初期化
  params.emplace_back(Parameter{0, 3, 1, 10, nullptr});   // Pr.9 初期化

  params.emplace_back(Parameter{0, 3, 1, 10, nullptr});   // Pr.10 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.11 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.12 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.13 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.14 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.15 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.16 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.17 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.18 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.19 初期化

  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.20 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.21 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.22 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.23 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.24 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.25 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.26 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.27 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.28 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.29 初期化

  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.30 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.31 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.32 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.33 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.34 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.35 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.36 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.37 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.38 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.39 初期化

  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.40 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.41 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.42 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.43 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.44 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.45 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.46 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.47 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.48 初期化
  params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.49 初期化

}

bool ParameterManager::setupParameter(uint8_t index, int defaultValue, int minValue, int maxValue, CallbackType callback) {
  if (index >= params.size()) {
    logError("setupParameter: Invalid index");
    return false;
  }
  if (minValue > maxValue) {
    logError("setupParameter: Min greater than Max");
    return false;
  }

  Parameter& param = params[index];
  param.defaultValue = defaultValue;
  param.minValue = minValue;
  param.maxValue = maxValue;
  param.onChanged = callback;

  std::cout << "setupParameter: defaultValue=" << static_cast<int>(defaultValue) << std::endl;
  std::cout << "setupParameter: currentValue=" << static_cast<int>(params[index].defaultValue) << std::endl;
  std::cout << "setupParameter: currentValue=" << static_cast<int>(param.defaultValue) << std::endl;

  uint8_t loadedValue = 0;
  bool loaded = storage.load(index, &loadedValue);

  std::cout << "setupParameter: loadedValue=" << static_cast<int>(loadedValue) << std::endl;

  if (!loaded || loadedValue < minValue || loadedValue > maxValue) {
    // EEPROM読み込み失敗 または 範囲外 → 初期値で復元
    param.currentValue = defaultValue;
    storage.save(index, defaultValue);

    char buf[80];
    snprintf(buf, sizeof(buf), "Param %u load fail or out of range, set to default", index);
    logError(buf);
  } else {
    param.currentValue = loadedValue;
  }

  return true;
}

bool ParameterManager::setParameter(uint8_t index, int value) {
//  Serial.printf("setParameter: index=%u, value=%d\n", index, value);
  if (index >= params.size()) {
    logError("setParameter: Invalid index");
    return false;
  }

  std::lock_guard<std::recursive_mutex> lock(eeprom->getMutex());

  Parameter& param = params[index];

  if (value < param.minValue || value > param.maxValue) {
    char buf[80];
    snprintf(buf, sizeof(buf), "Param %u set out of range (%d-%d)", index, param.minValue, param.maxValue);
    logError(buf);
    return false; // 範囲外エラー
  }

  if (param.currentValue != value) {
    param.currentValue = value;
    storage.save(index, value);

    if (param.onChanged) {
      param.onChanged(index, value); // コールバック発火
    }
  }
  return true; // 成功
}

int ParameterManager::getParameter(uint8_t index) {
  if (index >= params.size()) return 0;

  std::lock_guard<std::recursive_mutex> lock(eeprom->getMutex());

  int value = 0;
  value = params[index].currentValue;

  std::cout << "getParameter: index=" << static_cast<int>(index) << std::endl;
  std::cout << "getParameter: currentValue=" << static_cast<int>(params[index].currentValue) << std::endl;

  return params[index].currentValue;
}

void ParameterManager::clearAllParameters() {
  std::lock_guard<std::recursive_mutex> lock(eeprom->getMutex());

  for (uint8_t i = 0; i < params.size(); ++i) {
    params[i].currentValue = params[i].defaultValue;
    storage.save(i, params[i].currentValue);

    if (params[i].onChanged) {
      params[i].onChanged(i, params[i].currentValue); // 初期化でも通知
    }
  }

  logInfo("All parameters reset to default");
}

void ParameterManager::logError(const char* message) {
  if (logger) {
    logger->writeLog(message);
  }
}

void ParameterManager::logInfo(const char* message) {
  if (logger) {
    logger->writeLog(message);
  }
}
