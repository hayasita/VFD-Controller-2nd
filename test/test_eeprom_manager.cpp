#include <gtest/gtest.h>
#include "../src/EepromManager.h"
#include "mock/DummyI2CBusManager.h"  // 必要ならモックをインクルード

class DummyEepromRawAccess{
};

// テストフィクスチャ
class EepromManagerTest : public ::testing::Test {
protected:
  EepromManager eepromManager;
  DummyI2CBusManager mockBusManager;
  DummyEepromRawAccess mockEepromRawAccess;

  EepromManagerTest()
  : eepromManager(&mockBusManager),  // EepromManagerのコンストラクタにモックを渡す
    mockBusManager(),                // モックのI2CBusManagerを初期化
    mockEepromRawAccess()           // モックのEepromRawAccessを初期化
  {
    // コンストラクタで初期化
  }

  void SetUp() override {
    // 必要なら初期化
  //  eepromManager.begin();
  }

  void TearDown() override {
    // 必要なら後処理
  }
};

// サンプルテスト
TEST_F(EepromManagerTest, WriteAndReadBytes) {
//  uint8_t writeData[4] = {1, 2, 3, 4};
  uint8_t readData[4] = {0};

  EXPECT_TRUE(eepromManager.writeByte(0, 1));
  EXPECT_TRUE(eepromManager.readByte(0, &readData[0]));
  EXPECT_EQ(0, readData[0]);
}

// 他のテストケースもここに追加