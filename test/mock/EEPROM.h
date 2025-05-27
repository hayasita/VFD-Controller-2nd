#include <cstdint>
#include <cstring>  // memcpy 用

class EEPROMClassMock {
public:
  static const int SIZE = 1024;
  uint8_t memory[SIZE];

  // コンストラクタ
  EEPROMClassMock() {
      // メモリをゼロで初期化
      memset(memory, 0, SIZE);
  }

  // begin メソッドに引数を追加
  void begin(size_t size) {
    // EEPROMのサイズ設定や初期化を行う
    // 引数は実際のEEPROMサイズと一致させる
    memset(memory, 0, size);  // メモリを初期化
  }

  void get(int address, void* data, size_t size) {
    memcpy(data, &memory[address], size);  // memcpy を使用
  }

  void put(int address, const void* data, size_t size) {
    memcpy(&memory[address], data, size);  // memcpy を使用
  }

  uint8_t* getDataPtr() {
    return memory;  // メモリへのポインタを返す
  }

  // commit メソッドを追加
  bool commit() {
    // 通常のEEPROMはデータをフラッシュするが、モックでは何もしない
    return true;
  }
};

// グローバルにEEPROMを定義
extern EEPROMClassMock EEPROM;
