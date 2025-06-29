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
  : 
    realMonitorDeviseIo(),                            // シリアル入出力処理の初期化
    serialCommandProcessor(realMonitorDeviseIo, i2cBus, paramManager, eepromManager, wiFiManager),  // シリアルコマンド処理の初期化
    paramManager(&eepromManager, &logManager, &systemManager),  // パラメータ管理の初期化
    systemManager(),                                  // システム管理の初期化
    i2cBus(),        // I2Cバス管理の初期化
    display(&i2cBus),                                 // OLED表示の初期化
    eepromManager(&i2cBus),                           // EEPROM管理の初期化
    envSensor(&i2cBus),                               // 環境センサの初期化
    rtcManager(&i2cBus),                              // RTC管理の初期化
    builtInLedCtrl(builtInLeds, NUM_BUILTIN_LEDS),    // 内蔵LED制御の初期化
    externalLedCtrl(externalLeds, NUM_EXTERNAL_LEDS), // 外部LED制御の初期化
    jsonCommandProcessor(&paramManager, &wiFiManager, &systemManager),                // JSONコマンド処理の初期化
    wiFiManager(&wifiReal),                                                           // WiFi接続管理の初期化
    webServerManager(&paramManager, &jsonCommandProcessor, &wiFiManager)              // Webサーバ管理の初期化
{
  return;
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
  display.begin();                        // OLED表示の初期化
  eepromManager.begin();            // EEPROMの初期化
  logManager.begin(eepromManager);        // ログ管理の初期化
  rtcManager.begin();                     // RTCの初期化
  timeManager.begin(&rtcManager);         // 時間管理の初期化
  TimeManager::setInstance(&timeManager); // シングルトンインスタンス設定
//  timeManager.setSystemTimeManually(2023, 10, 1, 12, 0, 0); // 手動で時刻設定

  // 4. LEDの初期化
  FastLED.setBrightness(LED_MAX_BRIGHTNESS);                                            // 明るさを設定
  FastLED.addLeds<WS2812, BUILTIN_LED_DATA_PIN, GRB>(builtInLeds, NUM_BUILTIN_LEDS);    // 内蔵LEDの初期化
  FastLED.addLeds<WS2812, EXTERNAL_LED_DATA_PIN, GRB>(externalLeds, NUM_EXTERNAL_LEDS); // 外部LEDの初期化
//LED点灯テスト
  builtInLedCtrl.setMode(0, LedMode::Blink, CRGB::Red);
  externalLedCtrl.setMode(0, LedMode::On, CRGB::Green);
  externalLedCtrl.setMode(1, LedMode::Blink, CRGB::Blue);
  externalLedCtrl.setMode(2, LedMode::On, CRGB::Orange);

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

  // WiFi接続時のコールバック設定
  wiFiManager.onConnected([this](){
//    webServerManager.begin(&paramManager, &jsonCommandProcessor, &wiFiManager);  // WiFi接続後に開始
    webServerManager.begin();  // WiFi接続後に開始
    timeManager.configureSNTP();  // SNTP設定
  });

  // WiFi切断時のコールバック設定
  wiFiManager.onDisconnected([this]() {
    Serial.println("--WiFi disconnected");
    webServerManager.end();    // 切断時にサーバ停止（WebSocket含む）
  });

  // SNTP同期完了時のコールバック設定
  timeManager.onSntpSync([this]() {
    Serial.println("SNTP sync completed");
//    timeManager.updateRTCFromSystemTime();  // SNTP同期後にRTC更新
//    updateClockDisplay();  // OLEDに時刻表示
    wiFiManager.sntpCompleted = true;           // SNTP同期完了フラグ設定

  });

  // setting.js 生成コールバックの設定
  webServerManager.onMakeSettingJs([this]() -> std::string {
    Serial.println("./setting.js generated");
    return systemManager.makeSettingJs();  // システムマネージャからsetting.jsを生成
  });

  /*
  wiFiManager.connect("", "");
  
  if (wiFiManager.isConnected()) {
    Serial.println("wifi.isConnected.");
//    timeManager.syncWithSNTP("ntp.nict.jp", 9 * 3600);  // JST (UTC+9)
  }
*/

  systemManager.begin(wiFiManager, timeManager, paramManager);      // システム管理の初期化
  paramManager.begin();                                             // パラメータ管理の初期化 systemManagerの後に呼び出す必要がある

    // serialMonitor init
//  serialCommandProcessor = SerialCommandProcessor(&realMonitorDeviseIo);
   Serial.println("SystemController initialized");
}

void SystemController::update() {
//  uint8_t itmKeyCode;

  wiFiManager.update();
  webServerManager.update();

  // 端子入力
//  itmKeyCode = terminalInputManager.man();
  SystemEvent event = terminalInputManager.update();
  systemManager.update(event);  // システム管理の更新
  if (event != SystemEvent::None) {
    Serial.printf("Event: %d\n", static_cast<int>(event));
  }

  // シリアルモニタ処理
  serialCommandProcessor.exec();

  builtInLedCtrl.update();    // 内蔵LEDの更新処理
  externalLedCtrl.update();   // 外部LEDの更新処理

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

  timeinfo = timeManager.getLocalTimeStruct();
  display.showTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, 20, "LOC");  // ローカル時刻を表示

  timeinfo = timeManager.getRtcTimeStruct();  // RTC時刻を取得
  display.showTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, 30, "RTC");  // RTC時刻を表示

  time_t now = timeManager.getSystemTime();  // システム時刻を取得
  DateTime dt(now);
  display.showTime(dt.hour(), dt.minute(), dt.second(), 40, "SYS");  // システム時刻を表示

  return;   
}