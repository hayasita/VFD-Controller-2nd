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
  std::cout << "ParameterManager begin\n";
  params.clear();
  params.resize(MAX_PARAMS);  // パラメータ数を予約

  setupParameter(0, 1, 0,  1, nullptr);   // Pr.0 初期化
  setupParameter(1, 3, 1, 10, nullptr);   // Pr.1 初期化

/*
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
*/
}

/**
 * @brief パラメータの設定（EEPROM読み込み）
 * この関数は、指定されたインデックスのパラメータを初期化し、EEPROMから値を読み込む。
 * 初期値、最小値、最大値、および値変更時のコールバック関数を設定する。
 * @note
 * この関数は、パラメータのインデックスが有効であることを確認し、最小値が最大値以下であることを検証する。
 * もしEEPROMからの読み込みが失敗した場合、または読み込んだ値が範囲外である場合は、初期値を設定し、EEPROMに保存する。
 * 
 * @param index         パラメータのインデックス
 * @param defaultValue  初期値
 * @param minValue      最小値
 * @param maxValue      最大値
 * @param callback      値変更時のコールバック関数
 * @return true         成功
 * @return false        失敗
 */
bool ParameterManager::setupParameter(uint8_t index, int defaultValue, int minValue, int maxValue, CallbackType callback) {
  if (index >= params.size()) {
    logError("setupParameter: Invalid index");
    return false;
  }
  if (minValue > maxValue) {
    logError("setupParameter: Min greater than Max");
    return false;
  }

  // 設定値の設定
  Parameter& param = params[index];
  param.defaultValue = defaultValue;
  param.minValue = minValue;
  param.maxValue = maxValue;
  param.onChanged = callback;

//  std::cout << "setupParameter: index=" << static_cast<int>(index) << std::endl;
//  std::cout << "setupParameter: defaultValue0=" << static_cast<int>(defaultValue) << std::endl;
//  std::cout << "setupParameter: defaultValue1=" << static_cast<int>(params[index].defaultValue) << std::endl;
//  std::cout << "setupParameter: defaultValue2=" << static_cast<int>(param.defaultValue) << std::endl;

  // EEPROMから値を読み込む
  uint8_t loadedValue = 0;
  bool loaded = storage.load(index, &loadedValue);

//  std::cout << "setupParameter: loadedValue=" << static_cast<int>(loadedValue) << std::endl;

  if (!loaded || loadedValue < minValue || loadedValue > maxValue) {
    // EEPROM読み込み失敗 または 範囲外 → 初期値で復元
    param.currentValue = defaultValue;  // 初期値を設定
    storage.save(index, defaultValue);  // 初期値をEEPROMに保存

    char buf[80];
    snprintf(buf, sizeof(buf), "Param %u load fail or out of range, set to default", index);
    logError(buf);
    std::cout << buf << std::endl;
  } else {
    param.currentValue = loadedValue;   // 読み込んだ値を設定
  }
  std::cout << "setupParameter: index=" << static_cast<int>(index) << " : currentValue=" << static_cast<int>(param.currentValue) << std::endl;

  return true;
}

/**
 * @brief パラメータの設定（EEPROM書き込み）
 * この関数は、指定されたインデックスのパラメータに新しい値を設定し、EEPROMに保存する。
 * 値が範囲外の場合はエラーをログに記録し、設定を行わない。
 * 
 * @param index  パラメータのインデックス
 * @param value  設定する値
 * @return true  成功
 * @return false 範囲外エラー
 */
bool ParameterManager::setParameter(uint8_t index, uint8_t value) {
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

/**
 * @brief パラメータの取得（RAM読み込み）
 * この関数は、指定されたインデックスのパラメータの現在の値を取得する。
 * 
 * @param index  パラメータのインデックス
 * @return uint8_t  現在の値
 */
uint8_t ParameterManager::getParameter(uint8_t index) {
//  Serial.printf("getParameter: index=%u\n", index);
  if (index >= params.size()) return 0;

  std::lock_guard<std::recursive_mutex> lock(eeprom->getMutex());

  uint8_t value = 0;
  value = params[index].currentValue;

//  std::cout << "getParameter: index=" << static_cast<int>(index) << std::endl;
//  std::cout << "getParameter: currentValue=" << static_cast<int>(params[index].currentValue) << std::endl;

  return value;
}

/**
 * @brief パラメータの取得（EEPROM読み込み）
 * この関数は、指定されたインデックスのパラメータの現在の値をEEPROMから読み込む。
 * 
 */
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
