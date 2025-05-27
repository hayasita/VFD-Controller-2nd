#include "LogManager.h"

void LogManager::begin(EepromManager& eeprom) {
  this->eeprom = &eeprom;  // EepromManagerの参照を保持
  storage.begin(eeprom);   // ログストレージの初期化
  currentAddr = LOG_START_ADDR;  // ログ書き込みの初期アドレス設定
}

void LogManager::writeLog(const char* message) {
  std::lock_guard<std::recursive_mutex> lock(eeprom->getMutex());  // 排他制御
  size_t len = strnlen(message, MAX_LOG_LENGTH - 1);  // メッセージ長取得
  if (currentAddr + len + 1 > LOG_END_ADDR) {  // ログの終了アドレスを超える場合、開始アドレスに戻る
    currentAddr = LOG_START_ADDR;
  }
  storage.write(currentAddr, message, len + 1);  // ログの書き込み
  currentAddr += len + 1;  // 書き込みアドレス更新

}

bool LogManager::readFirstLog(char* buffer, size_t bufferSize) {
  std::lock_guard<std::recursive_mutex> lock(eeprom->getMutex());  // 排他制御
  return storage.read(LOG_START_ADDR, buffer, bufferSize);  // 最初のログの読み込み
}
