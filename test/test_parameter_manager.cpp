#include <gtest/gtest.h>
#include <iostream>
#include "./mock/DummyLogManager.h"
#include "./mock/DummyEepromManager.h"
//#include "./mock/I2CBusManager.h"  // モックのI2CBusManagerをインクルード
#include "../src/ParameterManager.h"  // ←実際のヘッダに合わせてパスを調整してね！

class ParameterManagerTest : public ::testing::Test {
protected:
  DummyEepromManager eepromManager;
  DummyLogManager logManager;
  ParameterManager paramManager;
  
  ParameterManagerTest() : paramManager(&eepromManager, &logManager) {
    // コンストラクタで初期化
  }
  void SetUp() override {
    logManager.begin(eepromManager);
    paramManager.begin();
    paramManager.setupParameter(0, 10, 5, 100);  // 初期値10, 5〜100
    paramManager.setParameter(0, 15); // 初期値15, 5〜100
  }

};

TEST_F(ParameterManagerTest, FirstTest) {
  int value = 10;
  EXPECT_EQ(value, 10);  // 初期値ロード確認
}

TEST_F(ParameterManagerTest, LoadDefaultValues) {
  paramManager.begin();
  paramManager.clearAllParameters();
  int value = paramManager.getParameter(1);
  EXPECT_EQ(value, 3);  // 初期値ロード確認
}

/**
 * @brief パラメータの設定と取得（設定範囲内）のテスト
 * - パラメータを設定し、正しく取得できることを確認する
 */
TEST_F(ParameterManagerTest, SetParameterWithinRange) {
  bool result = paramManager.setParameter(0, 50);   // 0番パラメータに50を設定
  EXPECT_TRUE(result);                              // 成功することを確認
  int value = paramManager.getParameter(0);         // 0番パラメータの値を取得
  EXPECT_EQ(value, 50);                             // 50が設定されていることを確認
}

/**
 * @brief パラメータの設定と取得（設定範囲外）のテスト
 * - 範囲外の値を設定し、エラーが発生することを確認する
 */
TEST_F(ParameterManagerTest, SetParameterOutOfRangeLow) {
  bool result = paramManager.setParameter(0, 0);    // 0番パラメータに0を設定（範囲外）
  EXPECT_FALSE(result);                             // 失敗することを確認
  EXPECT_NE(std::string::npos, logManager.lastMessage.find("out of range")); // ログに"範囲外"が記録されること
}

TEST_F(ParameterManagerTest, SetParameterOutOfRangeHigh) {
  bool result = paramManager.setParameter(0, 200);  // 0番パラメータに200を設定（範囲外）
  EXPECT_FALSE(result);                             // 失敗することを確認
  EXPECT_NE(std::string::npos, logManager.lastMessage.find("out of range")); // ログに"範囲外"が記録されること
}

/**
 * @brief EEPROM読み込み範囲外時、デフォルト値が設定されることを確認する
 */
TEST_F(ParameterManagerTest, SetupParameter_LoadFails_UseDefault) {
  static constexpr int PARAM_START_ADDR = 0x0010;  // パラメータの開始アドレス
  uint8_t value = 101;
  uint16_t index = 0;
  eepromManager.writeByte(PARAM_START_ADDR + index, value);   // MockEEPROMに書き込む
  uint8_t valueRead = 0;
  eepromManager.readByte(index, &valueRead);                  // MockEEPROMから読み込む
  std::cout << "EEPROM read value: " << static_cast<int>(valueRead) << std::endl; // 読み込んだ値を表示
  ASSERT_TRUE(paramManager.setupParameter(0, 42, 0, 100));    // Pr.0, デフォルト値42, 範囲0〜100 設定
  EXPECT_EQ(paramManager.getParameter(0), 42);                // デフォルト値が使用されることを確認
}

/**
* @brief EEPROMから正常に読み込めた場合、その値が使用されることを確認する
*/
TEST_F(ParameterManagerTest, SetupParameter_LoadSucceeds_WithinRange) {
  // 事前にMockEEPROMに値を書き込んでおく
  static constexpr int PARAM_START_ADDR = 0x0010;  // パラメータの開始アドレス
  int value = 55;
  uint8_t index = 1;
  eepromManager.writeByte(PARAM_START_ADDR + index, value); // MockEEPROMに書き込む
  ASSERT_TRUE(paramManager.setupParameter(1, 42, 0, 100));  // Pr.1, デフォルト値42, 範囲0〜100 設定
  EXPECT_EQ(paramManager.getParameter(1), 55);              // MockEEPROMから読み込んだ値が使用されることを確認
}

/**
* @brief setParameterで正常な範囲の値を設定できることを確認する
*/
TEST_F(ParameterManagerTest, SetParameter_Success) {
  paramManager.setupParameter(2, 0, -10, 10);
  ASSERT_TRUE(paramManager.setParameter(0, 5));
  EXPECT_EQ(paramManager.getParameter(0), 5);
}

/**
* @brief 下限未満の値を設定しようとしたとき、エラーになることを確認する
*/
TEST_F(ParameterManagerTest, SetParameter_OutOfRange_Low) {
  paramManager.setupParameter(3, 0, 0, 10);
  ASSERT_FALSE(paramManager.setParameter(3, -1)); // 低すぎる
  EXPECT_EQ(paramManager.getParameter(3), 0);      // 変更されていない
}

/**
* @brief 上限を超える値を設定しようとしたとき、エラーになることを確認する
*/
TEST_F(ParameterManagerTest, SetParameter_OutOfRange_High) {
  paramManager.setupParameter(4, 0, 0, 10);
  ASSERT_FALSE(paramManager.setParameter(4, 11)); // 高すぎる
  EXPECT_EQ(paramManager.getParameter(4), 0);     // 変更されていない
}

/**
* @brief setParameterで値を変更した際、コールバックが呼び出されることを確認する
*/
TEST_F(ParameterManagerTest, CallbackIsInvokedOnSetParameter) {
  bool callbackCalled = false;
  uint8_t callbackIndex = 255;
  int callbackValue = -1;

  // コールバックを登録してパラメータセット
  paramManager.setupParameter(5, 0, 0, 10, [&](uint8_t index, int newValue) {
      callbackCalled = true;
      callbackIndex = index;
      callbackValue = newValue;
  });

  ASSERT_TRUE(paramManager.setParameter(5, 7));

  // コールバックが呼び出されたことを確認
  EXPECT_TRUE(callbackCalled);
  EXPECT_EQ(callbackIndex, 5);
  EXPECT_EQ(callbackValue, 7);
}

/**
* @brief clearAllParametersを呼び出した際、コールバックが呼び出されることを確認する
*/
TEST_F(ParameterManagerTest, CallbackIsInvokedOnClearAllParameters) {
  bool callbackCalled = false;
  uint8_t callbackIndex = 255;
  int callbackValue = -1;

  // コールバックを登録してパラメータセット
  paramManager.setupParameter(6, 5, 0, 10, [&](uint8_t index, int newValue) {
      callbackCalled = true;
      callbackIndex = index;
      callbackValue = newValue;
  });

  // 値を初期値とは違うものにセットしておく
  paramManager.setParameter(6, 8);
  EXPECT_EQ(paramManager.getParameter(6), 8);

  callbackCalled = false; // リセットしてからclear

  // clearAllParametersを呼び出すと、デフォルトに戻りコールバックが呼び出される
  paramManager.clearAllParameters();

  EXPECT_TRUE(callbackCalled);
  EXPECT_EQ(callbackIndex, 6);
  EXPECT_EQ(callbackValue, 5); // デフォルト値
}