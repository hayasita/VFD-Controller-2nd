// ParameterManager.h
#pragma once

#include <functional>
#include <vector>
#include "EepromManager.h"
#include "LogManager.h"
#include "ParameterStorage.h"

/**
 * @brief パラメータ管理クラス
 * - パラメータごとに「初期値」「最小値」「最大値」を設定可能
 * - 値変更時にイベントコールバックを発火できる
 * - EEPROMを用いてパラメータの永続化を行う
 * - 範囲外エラー時にはログに記録される
 */
class ParameterManager {
public:
  using CallbackType = std::function<void(uint8_t index, int newValue)>;

  void begin(EepromManager& eeprom, LogManager& logger);  // 初期化
  bool setupParameter(uint8_t index, int defaultValue, int minValue, int maxValue, CallbackType callback = nullptr);  // パラメータの設定（EEPROM読み込み）
  bool setParameter(uint8_t index, int value);    // パラメータの設定（EEPROM書き込み）
  bool getParameter(uint8_t index, int& value);   // パラメータの取得（EEPROM読み込み）
  int getParameter(uint8_t index);                // パラメータの取得（EEPROM読み込み）
  void clearAllParameters();                      // すべて初期値に戻す（コールバックも発火）

private:
  struct Parameter {    // パラメータ構造体
    int currentValue;       // 現在の値
    int defaultValue;       // 初期値
    int minValue;           // 最小値
    int maxValue;           // 最大値
    CallbackType onChanged; // 値変更イベントコールバック
  };

  EepromManager* eeprom = nullptr;          // EepromManagerの参照
  LogManager* logger = nullptr;             // LogManagerの参照
  ParameterStorage storage;                 // パラメータストレージ
  std::vector<Parameter> params;            // パラメータのリスト
  static constexpr uint8_t MAX_PARAMS = 10; // 最大パラメータ数

  void logError(const char* message);
  void logInfo(const char* message);
};
