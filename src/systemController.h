#pragma once

#include "LittleFS.h"

#include "I2CBusManager.h"
#include "EepromManager.h"
#include "ParameterManager.h"
#include "LogManager.h"
#include "EnvironmentSensor.h"
#include "DisplayManager.h"       // OLED表示クラス
#include "RTCManager.h"
#include "TimeManager.h"        // 時間管理クラス
#include "WiFiManager.h"       // WiFi接続管理クラス
#include "WebServerManager.h"  // Webサーバ管理クラス
#include "TerminalInputManager.h" // 端子入力管理クラス
#include "SystemManager.h"      // システム管理クラス
#include "JsonCommandProcessor.h" // JSONコマンド処理クラス
#include "SerialCommandProcessorRealDevice.h" // シリアルコマンド処理クラス
#include "SerialCommandProcessor.h" // シリアルコマンド処理クラス

// システム全体の管理クラス
class SystemController {
public:
  SystemController(void);

  // 初期化
  void begin();

  // 更新処理
  void update();

private:
  I2CBusManager i2cBus;          // I2Cバス管理
  EepromManager eepromManager;   // EEPROM管理
  ParameterManager paramManager; // パラメータ管理
  LogManager logManager;         // ログ管理
  EnvironmentSensor envSensor;   // 環境センサ管理
  DisplayManager display;        // OLED表示管理
  RTCManager rtcManager;        // RTC管理
  TimeManager timeManager;      // 時間管理
  WiFiManager wiFiManager;                      // WiFi接続管理
  WebServerManager webServerManager;            // Webサーバ管理
  TerminalInputManager terminalInputManager;    // 端子入力管理
  SystemManager systemManager;                  // システム管理
  JsonCommandProcessor commandProcessor;        // JSONコマンド処理
  RealMonitorDeviseIo realMonitorDeviseIo; // シリアルコマンド処理
  SerialCommandProcessor serialCommandProcessor; // シリアルコマンド処理

  unsigned long lastReadTime = 0;  // 最後の読み込み時間
  const unsigned long readInterval = 1000;  // 読み込み間隔（ミリ秒）
  
  void updateClockDisplay();                   // OLEDに時刻表示
};
