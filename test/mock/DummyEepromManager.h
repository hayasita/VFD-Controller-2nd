/**
 * @file      DummyEepromManager.h
 * @brief     EepromManagerのモッククラス
 * @details   テスト用にEepromManagerの機能を模倣するクラス
 * @version   0.1
 * @author    2025／06／10
 * 
 * @copyright Copyright (c) 2025
 */

#pragma once

#include <iostream>
#include "../../src/EepromManager.h"  // 実際の EepromManager クラスのヘッダーファイルをインクルード

/**
 * @brief     EepromManagerのモッククラス
 * @details   テスト用にEepromManagerの機能を模倣するクラス
 *            実際のハードウェアに依存せず、メモリ上の配列にデータを格納する
 */
class DummyEepromManager : public EepromManager {
public:
  DummyEepromManager() {
    // コンストラクタで初期化は行わない
    // メモリ配列のサイズを定義
    memset(memory, 0, SIZE);  // メモリを初期化
  }

  ~DummyEepromManager() {
    // デストラクタで特に何もしない
    // メモリ配列は自動的に解放される
  }

/*
  void begin(I2CBusManager& busManager) override {
    // モックの初期化処理
//    memset(memory, 0, SIZE);  // メモリを初期化
  }
*/

  /**
   * @brief         EEPROMアクセス用のミューテックスを取得
   * @return        std::recursive_mutex& EEPROM専用のミューテックス
   */
  std::recursive_mutex& getMutex() override {
    return dummyMutex;
  }

  /**
   * @brief         データを1バイト書き込む
   * @param address 書き込みアドレス
   * @param data    書き込むデータ
   * @return        true 成功、false 失敗
   */
  bool writeByte(uint16_t address, const uint8_t data) override {
    if(address < 0 || address >= SIZE) {
      return false;  // 範囲外アクセスを防止
    }
    memory[address] = data;  // 単一バイトの書き込み
//    std::cout << "writeBytes: index=" << static_cast<int>(address) << std::endl;
//    std::cout << "writeBytes: memory=" << static_cast<int>(memory[address]) << std::endl;
//    std::cout << "writeBytes: data=" << static_cast<int>(data) << std::endl;
    return true;
  }

  bool readBytes(int address, void* data, size_t len) override {
    // モックの EEPROM 読み込み処理
    memcpy(data, &memory[address], len);  // memcpy を使用
    return true;
  }

  /**
   * @brief         データを1バイト読み出す
   * @param address 読み出しアドレス
   * @param data    読み出したデータを格納するポインタ
   * @return        true 成功、false 失敗
   */
  bool readByte(uint16_t address, uint8_t *data) override {
    if(address < 0 || address >= SIZE) {
      return false;  // 範囲外アクセスを防止
    }
    *data = memory[address];  // 単一バイトの読み込み
    return true;
  }

  bool readMultipleBytes(int address, uint8_t *data, size_t len) override {
    // モックの EEPROM 複数バイト読み込み処理
    memcpy(data, &memory[address], len);  // memcpy を使用
    return true;
  }

private:
  static const int SIZE = 1024;     // モックEEPROMのサイズ
  uint8_t memory[SIZE];             // モックEEPROMのメモリ配列
  std::recursive_mutex dummyMutex;  // モック用のミューテックス
};

