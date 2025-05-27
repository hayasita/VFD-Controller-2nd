#include <M5Unified.h>
#include "SystemController.h"
#include "Config.h"

/*
SystemController::SystemController()
  : realMonitorDeviseIo(),
    serialCommandProcessor(&realMonitorDeviseIo, i2cBus, eepromManager)
{
}
*/
SystemController::SystemController()
  : realMonitorDeviseIo(),
    serialCommandProcessor(realMonitorDeviseIo, i2cBus, eepromManager)
{
}

void SystemController::begin() {
  // 1. M5.begin() 設定（M5Unifiedを使う場合）
  auto cfg = M5.config();
  cfg.external_rtc  = true;  // default=false. use Unit RTC.
  M5.begin(cfg);
  M5.In_I2C.release();

  // 2. I2Cバス初期化
  i2cBus.begin();              // I2Cバスの初期化

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  // 3. 他モジュールの初期化
  eepromManager.begin(i2cBus);            // EEPROMの初期化
  logManager.begin(eepromManager);        // ログ管理の初期化
  paramManager.begin(eepromManager, logManager);  // パラメータ管理の初期化
  envSensor.begin(i2cBus);                // 環境センサの初期化
  display.begin(i2cBus);                  // OLED表示の初期化
  rtcManager.begin(i2cBus);               // RTCの初期化
  timeManager.begin(&rtcManager);         // 時間管理の初期化
  TimeManager::setInstance(&timeManager); // シングルトンインスタンス設定
//  timeManager.setSystemTimeManually(2023, 10, 1, 12, 0, 0); // 手動で時刻設定

  if (!rtcManager.isRunning()) {
//    logManager.writeLog("RTC not running!");
    display.showMessage("RTC Error");
    Serial.println("RTC not found");
  }
  else{
//    logManager.writeLog("RTC ready");
    display.showMessage("RTC ready");
    Serial.println("RTC ready");
  }

  // 端子入力初期化
  unsigned char swList[] = {BUTTON_0,BUTTON_1};
  terminalInputManager.begin(swList,sizeof(swList));


  wiFiManager.onConnected([this](){
    webServerManager.begin(&paramManager, &commandProcessor);  // WiFi接続後に開始
    timeManager.configureSNTP();  // SNTP設定
  });

  wiFiManager.onDisconnected([this]() {
    webServerManager.end();    // 切断時にサーバ停止（WebSocket含む）
  });

  /*
  wiFiManager.connect("", "");
  
  if (wiFiManager.isConnected()) {
    Serial.println("wifi.isConnected.");
//    timeManager.syncWithSNTP("ntp.nict.jp", 9 * 3600);  // JST (UTC+9)
  }
*/

  systemManager.begin(wiFiManager, timeManager);       // システム管理の初期化

    // serialMonitor init
//  serialCommandProcessor = SerialCommandProcessor(&realMonitorDeviseIo);

}

void SystemController::update() {
//  uint8_t itmKeyCode;

  wiFiManager.update();
//  webServerManager.update();  // 必要に応じて

  // 端子入力
//  itmKeyCode = terminalInputManager.man();
  SystemEvent event = terminalInputManager.update();
  systemManager.update(event);  // システム管理の更新
  if (event != SystemEvent::None) {
    Serial.printf("Event: %d\n", static_cast<int>(event));
  }

  // シリアルモニタ処理
  serialCommandProcessor.exec();

  if (millis() - lastReadTime >= readInterval) {  // 一定間隔でセンサデータを読み取る
/*
    if (envSensor.readSensorData()) {
      Serial.printf("Temp: %.1f, Hum: %.1f\n",
                    envSensor.getTemperature(),
                    envSensor.getHumidity());

      char logBuf[64];
      snprintf(logBuf, sizeof(logBuf), "T:%.1f H:%.1f",
               envSensor.getTemperature(), envSensor.getHumidity());
      logManager.writeLog(logBuf);  // ログ書き込み

    }
*/
//    if(rtcManager.isRunning()) {
      updateClockDisplay();  // RTCがあれば時刻表示
//      Serial.println("RTC running!");
//    } else {
//      Serial.println("RTC not running!");
//    }


    lastReadTime = millis();  // 最後の読み込み時間を更新
  }
}

void SystemController::updateClockDisplay() {
  struct tm timeinfo;
  timeinfo = timeManager.getSystemTimeStruct();
  display.showTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);  
  return;   
}