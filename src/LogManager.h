#pragma once

#include <string.h>
#include "LogStorage.h"
#include "EepromManager.h"

// 動作ログの管理クラス
class LogManager {
public:
  virtual ~LogManager() = default;

  // 初期化
  void begin(EepromManager& eeprom);

  // ログの書き込み
  virtual void writeLog(const char* message);

  // 最初のログを読み取る
  bool readFirstLog(char* buffer, size_t bufferSize);

private:
  EepromManager* eeprom = nullptr;  // EepromManagerの参照
  LogStorage storage;                // ログストレージ
  int currentAddr = 0;               // 現在の書き込みアドレス
  static constexpr int LOG_START_ADDR = 256;  // ログの開始アドレス
  static constexpr int LOG_END_ADDR = 512;    // ログの終了アドレス
  static constexpr size_t MAX_LOG_LENGTH = 64; // 最大ログ長
};
