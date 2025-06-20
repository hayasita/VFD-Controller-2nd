#include <gtest/gtest.h>

#include "./mock/MockSerialMonitorIO.h"
#include "../src/SerialCommandProcessor.h"
#include "../src/I2CBusManager.h"
#include "./mock/DummyEepromManager.h"
#include "./mock/DummyLogManager.h"
#include "./mock/MockWiFiManager.h"

namespace
{
  using ::testing::AtLeast;
  using ::testing::Return;
  using ::testing::Test;

  class DummyI2CBusManager : public I2CBusManager {
    public:
      void begin() override {}
      std::recursive_mutex& getMutex() override { return dummyMutex; }
      TwoWire& getWire() override { return wire; }
      std::vector<uint8_t> scanI2CBus(uint8_t address = 0x00, uint8_t count = 127) override {
        return {}; // モックなので空のリストを返す
      }
    private:
      std::recursive_mutex dummyMutex;
      TwoWire wire = TwoWire(0);  // 0番ポートを使用（ESP32）
  };

  class SerialMonitorTest : public Test {
    protected:
//      MockSerialMonitorIO mock;
//      SerialCommandProcessor serialMonitor = SerialMonitor(&mock);
    protected:
      DummyEepromManager eepromManager;
      DummyLogManager logManager;
      MockSerialMonitorIO mock;
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
        serialMonitor(mock, i2cbusManager, paramManager, eepromManager, wifiManager)
    {}

      SerialCommandProcessor serialMonitor{mock ,i2cbusManager ,paramManager ,eepromManager, wifiManager}; // ← MockSerialMonitorIOのポインタを渡す
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