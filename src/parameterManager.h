// ParameterManager.h
#pragma once

#include <functional>
#include <vector>
#include "EepromManager.h"
#include "LogManager.h"
#include "SystemManager.h"
#include "ParameterStorage.h"

#define BR_DEF        9   // 輝度初期値
#define BR_MAX        15  // 最大輝度
#define BR_MIN        1   // 最小輝度
#define ADJ_BR1       0

#define FADETIME_DEF  2   // クロスフェード時間初期値

/**
 * @brief パラメータ管理クラス
 * - パラメータごとに「初期値」「最小値」「最大値」を設定可能
 * - 値変更時にイベントコールバックを発火できる
 * - EEPROMを用いてパラメータの永続化を行う
 * - 範囲外エラー時にはログに記録される
 */
class ParameterManager {
public:
  ParameterManager(EepromManager *eeprom, LogManager *logger, SystemManager *systemManager = nullptr); // コンストラクタ
  ~ParameterManager();

  using CallbackType = std::function<void(uint8_t index, int newValue)>;

  void begin(void);                               // パラメータ群の初期設定
  bool setupParameter(uint8_t index, int defaultValue, int minValue, int maxValue, CallbackType callback = nullptr);  // パラメータの設定（EEPROM読み込み）
  bool setParameter(uint8_t index, uint8_t value);    // パラメータの設定（EEPROM書き込み）
  bool getParameter(uint8_t index, uint8_t& value);   // パラメータの取得（EEPROM読み込み）
  uint8_t getParameter(uint8_t index);                // パラメータの取得（EEPROM読み込み）
  void clearAllParameters();                          // すべて初期値に戻す（コールバックも発火）

private:
  struct Parameter {    // パラメータ構造体
    int currentValue;       // 現在の値
    int defaultValue;       // 初期値
    int minValue;           // 最小値
    int maxValue;           // 最大値
    CallbackType onChanged; // 値変更イベントコールバック
  };

  EepromManager *eeprom = nullptr;          // EepromManagerの参照
  LogManager *logger = nullptr;             // LogManagerの参照
  SystemManager *systemManager = nullptr; // SystemManagerの参照（必要に応じて追加）
  ParameterStorage storage;                 // パラメータストレージ
  std::vector<Parameter> params;            // パラメータのリスト
//  static constexpr uint8_t MAX_PARAMS = 10; // 最大パラメータ数

  void logError(const char* message);
  void logInfo(const char* message);
};
