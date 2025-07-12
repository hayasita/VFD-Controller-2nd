#include <gtest/gtest.h>

#include "./mock/MockSerialMonitorIO.h"
#include "../src/SerialCommandProcessor.h"
#include "../src/I2CBusManager.h"
#include "./mock/DummyEepromManager.h"
#include "./mock/DummyLogManager.h"
#include "./mock/MockWiFiManager.h"
#include "./mock/DummyI2CBusManager.h"
#include "./mock/DummySystemManager.h"  // モックのSystemManagerをインクルード

namespace
{
  using ::testing::AtLeast;
  using ::testing::Return;
  using ::testing::Test;

  class SerialMonitorTest : public Test {
    protected:
//      MockSerialMonitorIO mock;
//      SerialCommandProcessor serialMonitor = SerialMonitor(&mock);
    protected:
      DummyEepromManager eepromManager;
      DummyLogManager logManager;
      MockSerialMonitorIO mock;
      DummySystemManager dummySystemManager;
      DummyI2CBusManager i2cbusManager;  // I2Cバスマネージャのモック
      MockWiFiManager wifiManager;
      ParameterManager paramManager;

    SerialMonitorTest()
      : 
        i2cbusManager(),
        eepromManager(&i2cbusManager),  // I2CBusManagerを渡す
        logManager(),
        wifiManager(),
        paramManager(&eepromManager, &logManager),
        serialMonitor(mock, i2cbusManager, paramManager, eepromManager, wifiManager, &dummySystemManager) // SerialCommandProcessorのインスタンスを作成
    {}

      SerialCommandProcessor serialMonitor{mock ,i2cbusManager ,paramManager ,eepromManager, wifiManager, &dummySystemManager}; // ← MockSerialMonitorIOのポインタを渡す
  };
  std::vector<std::string> result = {"command","parameta1","parameta2"};
  std::vector<std::string> result2 = {"command"};
  std::string command = "command parameta1 parameta2";
  std::string command2 = "command";
  std::string dummyExec = "dummyExec";

  TEST_F(SerialMonitorTest, API)
  {
    EXPECT_EQ(result, serialMonitor.splitCommand(command));   // コマンド分割
    EXPECT_EQ(result2, serialMonitor.splitCommand(command2));   // コマンド分割
  }
  

  TEST_F(SerialMonitorTest, command)
  {
//    EXPECT_CALL(mock, rsv()).Times(AtLeast(1)).WillOnce(Return(command));           // テスト入力
//    EXPECT_CALL(mock, send(dummyExec + "\n")).Times(AtLeast(1)).WillOnce(Return(1));  // テスト出力
//    EXPECT_EQ(true, serialMonitor.exec()); // 期待値：true

    EXPECT_CALL(mock, rsv()).Times(AtLeast(1)).WillOnce(Return("command2"));           // テスト入力
    EXPECT_CALL(mock, send("dummyExec\n")).Times(AtLeast(1)).WillOnce(Return(1));  // テスト出力
    EXPECT_EQ(true, serialMonitor.exec()); // 期待値：true
  }

  int main(int argc, char **argv)
  {
    // 以下の行は，テスト開始前に Google Mock （と Google Test）
    // を初期化するために必ず実行する必要があります．
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
  }

} // namespace