#include <gtest/gtest.h>
#include "../src/EepromManager.h"
//#include "mock/MockI2CBusManager.h"  // 必要ならモックをインクルード

// テストフィクスチャ
class EepromManagerTest : public ::testing::Test {
protected:
    EepromManager eepromManager;
    MockI2CBusManager mockBusManager;

    void SetUp() override {
        // 必要なら初期化
        eepromManager.begin(mockBusManager);
    }

    void TearDown() override {
        // 必要なら後処理
    }
};

// サンプルテスト
TEST_F(EepromManagerTest, WriteAndReadBytes) {
    uint8_t writeData[4] = {1, 2, 3, 4};
    uint8_t readData[4] = {0};

    EXPECT_TRUE(eepromManager.writeBytes(0, writeData, 4));
    EXPECT_TRUE(eepromManager.readBytes(0, readData, 4));
    EXPECT_EQ(0, memcmp(writeData, readData, 4));
}

// 他のテストケースもここに追加