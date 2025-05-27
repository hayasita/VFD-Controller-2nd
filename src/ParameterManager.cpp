// ParameterManager.cpp
#include "ParameterManager.h"
#include <cstdio> // snprintf

void ParameterManager::begin(EepromManager& eeprom, LogManager& logger) {
  this->eeprom = &eeprom;
  this->logger = &logger;
  storage.begin(eeprom);

  params.clear();
  params.reserve(MAX_PARAMS);
//  for (uint8_t i = 0; i < MAX_PARAMS; ++i) {
    params.emplace_back(Parameter{0, 0, 0, 0, nullptr});    // Pr.0 初期化
    params.emplace_back(Parameter{0, 3, 1, 10, nullptr});   // Pr.1 初期化
    params.emplace_back(Parameter{0, 3, 1, 10, nullptr});   // Pr.2 初期化
    params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.3 初期化
    params.emplace_back(Parameter{0, 0, 0, 10, nullptr});   // Pr.4 初期化
    params.emplace_back(Parameter{0, 3, 1, 10, nullptr});   // Pr.5 初期化
    params.emplace_back(Parameter{0, 3, 1, 10, nullptr});   // Pr.6 初期化

//  }
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

  int loadedValue = 0;
  bool loaded = storage.load(index, loadedValue);

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
