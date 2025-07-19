#include <gtest/gtest.h>
#include <iostream>
#include "./mock/DummyLogManager.h"
#include "./mock/DummyEepromManager.h"
#include "./mock/DummyI2CBusManager.h"  // モックのI2CBusManagerをインクルード
#include "./mock/DummySystemManager.h"  // モックのSystemManagerをインクルード
#include "./mock/DummyTimeManager.h"    // モックのTimeManagerをインクルード
#include "./mock/DummyTerminalInputManager.h" // モックのTerminalInputManagerをインクルード
#include "../src/ParameterManager.h"    // テスト対象のParameterManagerをインクルード

// テスト用のモッククラスを使用してParameterManagerのテストを行う
class ParameterManagerTest : public ::testing::Test {
protected:
  DummyI2CBusManager dummyBusManager;  // I2CBusManagerのモック
  DummyEepromManager dummyEepromManager;
  DummyLogManager dummyLogManager;
  ParameterManager paramManager;
  DummySystemManager dummySystemManager;
  DummyTimeManager dummyTimeManager;        // モックのTimeManager

  ParameterManagerTest()
  : dummyBusManager(),
    dummyEepromManager(&dummyBusManager),  // I2CBusManagerのモックを渡す
    dummyLogManager(),
    paramManager(&dummyEepromManager, &dummyLogManager, &dummySystemManager),  // ParameterManagerのコンストラクタにモックを渡す
    dummyTimeManager(),                     // TimeManagerのモックを渡す
    dummySystemManager()
  {
    // コンストラクタで初期化
  }

  void SetUp() override {
    dummyLogManager.begin(dummyEepromManager);
    paramManager.begin();
//    paramManager.setupParameter(0, 10, 5, 100, nullptr);  // 初期値10, 5〜100
//    paramManager.setParameter(0, 15); // 初期値15, 5〜100
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
  bool result = paramManager.setParameter(1, 5);    // 1番パラメータに5を設定
  EXPECT_TRUE(result);                              // 成功することを確認
  int value = paramManager.getParameter(1);         // 0番パラメータの値を取得
  EXPECT_EQ(value, 5);                              // 50が設定されていることを確認
}

/**
 * @brief パラメータの設定と取得（設定範囲外）のテスト
 * - 範囲外の値を設定し、エラーが発生することを確認する
 */
TEST_F(ParameterManagerTest, SetParameterOutOfRangeLow) {
  bool result = paramManager.setParameter(1, 0);    // 1番パラメータに0を設定（範囲外）
  EXPECT_FALSE(result);                             // 失敗することを確認
  EXPECT_NE(std::string::npos, dummyLogManager.lastMessage.find("out of range")); // ログに"範囲外"が記録されること
}

TEST_F(ParameterManagerTest, SetParameterOutOfRangeHigh) {
  bool result = paramManager.setParameter(0, 200);  // 0番パラメータに200を設定（範囲外）
  EXPECT_FALSE(result);                             // 失敗することを確認
  EXPECT_NE(std::string::npos, dummyLogManager.lastMessage.find("out of range")); // ログに"範囲外"が記録されること
}

/**
 * @brief EEPROM読み込み範囲外時、デフォルト値が設定されることを確認する
 */
TEST_F(ParameterManagerTest, SetupParameter_LoadFails_UseDefault) {
  static constexpr int PARAM_START_ADDR = 0x0010;  // パラメータの開始アドレス
  uint8_t value = 101;
  uint16_t index = 0;
  dummyEepromManager.writeByte(PARAM_START_ADDR + index, value);   // MockEEPROMに書き込む
  uint8_t valueRead = 0;
  dummyEepromManager.readByte(index, &valueRead);                  // MockEEPROMから読み込む
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
  dummyEepromManager.writeByte(PARAM_START_ADDR + index, value); // MockEEPROMに書き込む
  ASSERT_TRUE(paramManager.setupParameter(1, 42, 0, 100));  // Pr.1, デフォルト値42, 範囲0〜100 設定
  EXPECT_EQ(paramManager.getParameter(1), 55);              // MockEEPROMから読み込んだ値が使用されることを確認
}

/**
* @brief setParameterで正常な範囲の値を設定できることを確認する
*/
TEST_F(ParameterManagerTest, SetParameter_Success) {
  paramManager.setupParameter(2, 0, -10, 10);
  ASSERT_TRUE(paramManager.setParameter(2, 5));
  EXPECT_EQ(paramManager.getParameter(2), 5);
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

// パラメータの境界値テストケース
struct ParamBoundaryTestCase {
    uint8_t paramIndex;
    int defaultValue;
    int belowMin;
    int atMin;
    int atMax;
    int aboveMax;
};

const std::vector<ParamBoundaryTestCase> boundaryTestCases = {
// Pr. default belowMin atMin atMax aboveMax
  { 0, 0x01,  -1,    0,      1,      2    }, // Pr.0 初期化 輝度
  { 1, 3,      0x00, 0x01,   10,     11   }, // Pr.1 初期化 クロスフェード時間
  { 2, 0x00,  -0x01, 0x00,   0x0A,   0x0B }, // Pr.2 初期化 時刻表示フォーマット
  { 3, 0,     -1,    0,      10,     11   }, // Pr.3 初期化 日付表示フォーマット
  { 4, 0,     -1,    0,      10,     11   }, // Pr.4 初期化 表示効果
  { 5, 2,     -1,    0,      9,      10   }, // Pr.5 初期化 クロスフェード時間
  { 6, 0x55,   0x31, 0x32,   0x64,   0x65 }, // Pr.6 初期化 明るさ設定
  { 7, 0x40,   0x31, 0x32,   0x64,   0x65 }, // Pr.7 初期化 暗さ設定
  { 8, BR_DEF, 0x00, BR_MIN, BR_MAX, 0x10 }, // Pr.8 初期化 輝度
  { 9, BR_DEF, 0x00, BR_MIN, BR_MAX, 0x10 }, // Pr.9 初期化 輝度
  {10, BR_DEF, 0x00, BR_MIN, BR_MAX, 0x10 }, // Pr.10 初期化 輝度
  {11, BR_DEF, 0x00, BR_MIN, BR_MAX, 0x10 }, // Pr.11 初期化 輝度
  {12, BR_DEF, 0x00, BR_MIN, BR_MAX, 0x10 }, // Pr.12 初期化 輝度
  {13, BR_DEF, 0x00, BR_MIN, BR_MAX, 0x10 }, // Pr.13 初期化 輝度
  {14, BR_DEF, 0x00, BR_MIN, BR_MAX, 0x10 }, // Pr.14 初期化 輝度
  {15, BR_DEF, 0x00, BR_MIN, BR_MAX, 0x10 }, // Pr.15 初期化 輝度
  {16, BR_DEF, 0x00, BR_MIN, BR_MAX, 0x10 }, // Pr.16 初期化 輝度

  {32, 0x00,  -1,    0x00,   0x01,   0x02 }, // Pr.32 初期化 WiFi自動接続
  {33, 0x04,  -1,    0x00,   0xFE,   0xFF }, // Pr.33 初期化 タイムゾーンエリアID
  {34, 0x50,  -1,    0x00,   0xFE,   0xFF }, // Pr.34 初期化 タイムゾーンID
  {35, 0x1E,  -1,    0x00,   0xFE,   0xFF }, // Pr.35 初期化 タイムゾーン
  {36, 0x00,  -1,    0x00,   0x17,   0x18 }, // Pr.36 初期化 SNTP auto update time 時
  {37, 0x00,  -1,    0x00,   0x3B,   0x3C }, // Pr.37 初期化 SNTP auto update time 分

  {43, 0x00,  -1,    0x00,   0x03,   0x04 }, // Pr.43 初期化 地域設定
  {44, 0x00,  -1,    0x00,   0x01,   0x02 }, // Pr.44 初期化 WiFi Station 設定：STA自動接続有効

  // 必要に応じて追加
};

class ParameterManagerBoundaryTest : public ParameterManagerTest, public ::testing::WithParamInterface<ParamBoundaryTestCase> {};

TEST_P(ParameterManagerBoundaryTest, BoundaryTest) {
  const auto& tc = GetParam();
  paramManager.clearAllParameters();

  // 下限未満
  EXPECT_FALSE(paramManager.setParameter(tc.paramIndex, tc.belowMin));
  EXPECT_EQ(paramManager.getParameter(tc.paramIndex), tc.defaultValue); // デフォルト値が設定されていることを確認

  // 下限ちょうど
  EXPECT_TRUE(paramManager.setParameter(tc.paramIndex, tc.atMin));
  EXPECT_EQ(paramManager.getParameter(tc.paramIndex), tc.atMin);

  // 上限ちょうど
  EXPECT_TRUE(paramManager.setParameter(tc.paramIndex, tc.atMax));
  EXPECT_EQ(paramManager.getParameter(tc.paramIndex), tc.atMax);

  // 上限超過
  EXPECT_FALSE(paramManager.setParameter(tc.paramIndex, tc.aboveMax));
  EXPECT_EQ(paramManager.getParameter(tc.paramIndex), tc.atMax);
}

INSTANTIATE_TEST_SUITE_P(
  ParamBoundary,
  ParameterManagerBoundaryTest,
  ::testing::ValuesIn(boundaryTestCases)
);
