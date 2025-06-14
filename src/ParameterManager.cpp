// ParameterManager.cpp
#include "ParameterManager.h"
#include <cstdio> // snprintf
#include <iostream>

#define MAX_PARAMS 50  // 最大パラメータ数

ParameterManager::ParameterManager(EepromManager *eeprom, LogManager *logger, SystemManager *systemManager)
  : eeprom(eeprom),     // EepromManagerの参照を初期化
    logger(logger),      // LogManagerの参照を初期化
    systemManager(systemManager), // SystemManagerの参照を初期化
    storage(eeprom)      // パラメータストレージを初期化
{
  // コンストラクタで初期化は行わない
}

ParameterManager::~ParameterManager() {}

void ParameterManager::begin(void) {
  std::cout << "ParameterManager begin\n";
  params.clear();

  Parameter defaultParam = {0, 0, 0, 0, nullptr}; // 任意の初期値
  params.resize(MAX_PARAMS, defaultParam);        // パラメータ数を予約 すべてこの値で埋める

  setupParameter( 0, 1, 0,  1, std::bind(&SystemManager::onParameterChanged, systemManager,  0, std::placeholders::_2));   // Pr.0 初期化 表示フォーマット：時刻表示12/24
  setupParameter( 1, 3, 1, 10, std::bind(&SystemManager::onParameterChanged, systemManager,  1, std::placeholders::_2));   // Pr.1 初期化 表示フォーマット：Display Format

  setupParameter(30, 0, 0,  1, std::bind(&SystemManager::onParameterChanged, systemManager, 30, std::placeholders::_2));   // Pr.30 初期化 SNTP設定：SNTP使用

  setupParameter(44, 0, 0,  1, std::bind(&SystemManager::onParameterChanged, systemManager, 44, std::placeholders::_2));   // Pr.44 初期化 WiFi Station 設定：STA自動接続有効

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
  std::cout << "ParameterManager::clearAllParameters\n";

  for (uint8_t i = 0; i < params.size(); ++i) {
    setParameter(i, params[i].defaultValue);
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
