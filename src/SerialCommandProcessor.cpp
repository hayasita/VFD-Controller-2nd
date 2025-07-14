/**
 * @file monitor.cpp
 * @author hayasita04@gmail.com
 * @brief シリアルモニタ処理
 * @version 0.1
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdint>

#include "SerialCommandProcessor.h"
extern "C" {
  #include "esp_system.h"
}

#ifdef UNIT_TEST
#else
#include "LittleFS.h"
#include <FS.h>
#include <esp_task_wdt.h>
#endif
//#include "jsdata.h"
#include "Config.h"
#include "ver.h"

/**
 * @brief Destroy the Serial Monitor I O:: Serial Monitor I O object
 * 
 */
MonitorDeviseIo::~MonitorDeviseIo() {}

/**
 * @brief Construct a new Serial Monitor:: Serial Monitor object
 * 
 * @param pMonitorDeviseIo 
 */
/*
 SerialCommandProcessor::SerialCommandProcessor(MonitorDeviseIo* pMonitorDeviseIo, I2CBusManager& busManager, EepromManager& eepromManager){
  monitorIo_ = pMonitorDeviseIo;
  i2cBus = &busManager;
  eeprom = &eepromManager;
  init();
}
*/
SerialCommandProcessor::SerialCommandProcessor(MonitorDeviseIo& MonitorDeviseIo, I2CBusManager& busManager, ParameterManager& parameterManager, EepromManager& eepromManager, WiFiManager& wifiManager, SystemManager *systemManager)
  : monitorIo_(&MonitorDeviseIo),         // シリアル入出力処理ポインタを設定
    i2cBus(&busManager),                  // I2CBusManagerの参照を設定
    parameterManager(&parameterManager),  // ParameterManagerの参照を設定
    eeprom(&eepromManager),               // EepromManagerの参照を設定
    wiFiManager(&wifiManager),             // WiFiManagerの参照を設定
    systemManager(systemManager) // SystemManagerの参照を初期化
{
  init();
}

void SerialCommandProcessor::init(void)
{
//  std::cout << "SerialCommandProcessor::init";
  codeArray.clear();    // コマンド実行テーブルクリア
  codeArray.push_back({"help"       ,[this](){return opecodeHelp(command);}    ,"Help\tDisplays help information for the command."});
  codeArray.push_back({"ls"         ,[this](){return opecodels(command);}  ,"ls path"});
  codeArray.push_back({"datalist"   ,[this](){return opecodedatalist(command);}  ,"datalist\tDisplays Matrix data file list."});
  codeArray.push_back({"env"        ,[this](){return opecodeenv(command);}  ,"env\tconfig data list."});
  codeArray.push_back({"ver"        ,[this](){return opecodeVer(command);}  ,"ver\tVersion."});
  codeArray.push_back({"timelength" ,[this](){return opecodeGetTimeLength(command);}  ,"timelength\tGet time length."});
  codeArray.push_back({"command2"   ,[this](){return dummyExec(command);}  ,"command Help."});

  codeArray.push_back({"eepromdump" ,[this](){ return opecodeEepromDump(command); }, "eepromdump\tEEPROM Data dump."});
  codeArray.push_back({"i2cscan"    ,[this](){ return opecodeI2CScan(command); }, "i2cscan\tI2C Bus Device Scan."});
  codeArray.push_back({"wifiscan"   ,[this](){ return opecodeWiFiScan(command); }, "wifiscan\tWiFi Station SSID Scan."});

  codeArray.push_back({"getpr"      ,[this](){ return opecodeGetPr(command); }, "getpr [Pr number]\t"});  // ダミーコマンド
  codeArray.push_back({"setpr"      ,[this](){ return opecodeSetPr(command); }, "setpr [Pr number] [value]\t"});  // ダミーコマンド

  return;
}

/**
 * @brief シリアルモニタ実行
 * 
 * @return true コマンド実行成功
 * @return false コマンド実行失敗
 */
bool SerialCommandProcessor::exec(void)
{
  std::string commandBuf;
//  std::vector<std::string> command;     // シリアルモニタコマンド
  bool ret = true;

  try {
    commandBuf = monitorIo_->rsv();

    if(commandBuf.size() > 0){
      commandBuf.erase(std::remove(commandBuf.begin(), commandBuf.end(), '\r'), commandBuf.end());    // CRを取り除く
      commandBuf.erase(std::remove(commandBuf.begin(), commandBuf.end(), '\n'), commandBuf.end());    // LFを取り除く
    }
    if(commandBuf.size() > 0){
      command = splitCommand(commandBuf);   // コマンドをトークンごとに分割
  /*      
      for (const auto &str : command) {
        std::cout << str << std::endl;
      }
  */
      std::vector<codeTbl>::iterator itr = std::find_if(codeArray.begin(),codeArray.end(),[&](codeTbl &c) {   // コマンド実行テーブル検索
        return(c.code == command[0]);
      });
      if(itr != codeArray.end()){
  /*
        std::cout << "テーブル検索成功\n";
        std::cout << (*itr).code << ":" << command[0] << ":\n";
        for (char c : command[0]) {
        // 1文字ごとのASCIIコードを出力
          std::cout << (int)c << "\n";
        }
  */
        ret = (*itr).execCode();    // コマンド実行
      }
      else{
        // テーブル検索失敗
        std::cout << "テーブル検索失敗";
        monitorIo_->send(command[0] + ": command not found.\n");
        ret = false;
      }
    }

  } catch(const std::exception& e) {
    monitorIo_->send(std::string("Exception: ") + e.what());
    ret = false;
  } catch(...) {
    monitorIo_->send("Unknown exception occurred.\n");
    ret = false;
  }

  return ret;
}

/**
 * @brief コマンド分割
 * 入力されたコマンドを半角スペースで分割して、std::vectorに格納する。
 * 
 * @param commandBuf 入力コマンド
 * @return std::vector<std::string> 分割したコマンド
 */
std::vector<std::string> SerialCommandProcessor::splitCommand(const std::string &commandBuf)    // コマンド分割
{
  std::istringstream iss(commandBuf);
  std::string token;
  std::vector<std::string> result;

  while (std::getline(iss, token, ' ')) {
    if(token.size() != 0){
      result.push_back(token);
    }
  }
/*
  for (const auto &str : result) {
    std::cout << str << std::endl;
  }
*/
  return result;
}


bool SerialCommandProcessor::dummyExec(std::vector<std::string> command)
{
  std::cout << "SerialCommandProcessor::dummyExec\n";
  monitorIo_->send("dummyExec\n");
  return true;
}

/**
 * @brief Helpコマンド処理
 * 
 * @param command 
 * @return true 
 * @return false 
 */
bool SerialCommandProcessor::opecodeHelp(std::vector<std::string> command)   // help
{
//  monitorIo_->send("help\n");

  for (auto it = codeArray.begin(); it != codeArray.end(); ++it) {
    std::cout << it->help << "\n";
  }

  return true;
}

#ifndef UNIT_TEST
void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    esp_task_wdt_reset();  // ウォッチドッグをリセット
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.print (file.name());
      time_t t= file.getLastWrite();
      struct tm * tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
      if(levels){
        listDir(fs, file.path(), levels -1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.print(file.size());
      time_t t= file.getLastWrite();
      struct tm * tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
    }
    file = root.openNextFile();
  }

  return;
}
#endif

bool SerialCommandProcessor::opecodels(std::vector<std::string> command_)     // ls
{
  monitorIo_->send("opecodels\n");
#ifndef UNIT_TEST

  for (const auto &str : command) {
    std::cout << str << std::endl;
  }
  if(command.size() > 1){
    listDir(LittleFS, command[1].c_str(), 0);
  }
  else{
    listDir(LittleFS, "/", 0);
  }
#endif

  return true;
}

bool SerialCommandProcessor::opecodedatalist(std::vector<std::string> command)   // datalist
{

//  std::for_each(jsData.dataFilePath.begin(), jsData.dataFilePath.end(), [](std::string x) {
//    Serial.println(x.c_str());
//  });

  return true;
}

/**
 * @brief 設定値表示
 * 
 * @param command 
 * @return true 
 * @return false 
 */
bool SerialCommandProcessor::opecodeenv(std::vector<std::string> command)
{
  monitorIo_->send("opecodeenv\n");
  std::string js = systemManager->makeSettingJs();  // ./setting.jsを生成する
  monitorIo_->send(js);  // 生成した設定情報を送信

  return true;
}

bool SerialCommandProcessor::opecodeVer(std::vector<std::string> command)        // バージョン表示
{
  monitorIo_->send("opecodeVer\n");
 
  // SW_VERSIONはver.hで定義されているバージョン文字列
  std::ostringstream oss;
  oss << "Version: " << SW_VERSION << "\n";
  std::string versionStr = oss.str();
  monitorIo_->send(versionStr);         // バージョン情報を送信

  // ESP32のボード情報を表示
  uint8_t mac[6] = {0,0,0,0,0,0};
  uint32_t chipId = 0;

  #ifndef UNIT_TEST
  // ESP.getEfuseMac() からMACアドレスを取得
  uint64_t efuseMac = ESP.getEfuseMac();
  for (int i = 0; i < 6; ++i) {
    mac[5 - i] = (efuseMac >> (8 * i)) & 0xFF;
  }
  #endif

  std::ostringstream ossMac;
  ossMac << "MAC Address: "
        << std::uppercase << std::hex << std::setfill('0')
        << std::setw(2) << static_cast<int>(mac[0]) << ":"
        << std::setw(2) << static_cast<int>(mac[1]) << ":"
        << std::setw(2) << static_cast<int>(mac[2]) << ":"
        << std::setw(2) << static_cast<int>(mac[3]) << ":"
        << std::setw(2) << static_cast<int>(mac[4]) << ":"
        << std::setw(2) << static_cast<int>(mac[5]) << "\n";
  std::string macStr = ossMac.str();
  monitorIo_->send(macStr);

  std::ostringstream ossChip;
  ossChip << "M5Stack Serial Number: "
          << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << chipId << "\n";
  std::string chipIdStr = ossChip.str();
  monitorIo_->send(chipIdStr);

  return true;
}

// 0xで始まる場合は16進数、それ以外は10進数で変換する関数
int parseStringToInt(const std::string& str) {
    int value = 0;
    std::istringstream iss(str);
    if (str.size() > 2 && (str[0] == '0') && (str[1] == 'x' || str[1] == 'X')) {
        iss >> std::hex >> value;
    } else {
        iss >> std::dec >> value;
    }
    return value;
}

bool SerialCommandProcessor::opecodeGetTimeLength(std::vector<std::string> command) {
  monitorIo_->send("> opecodeGetTimeLength\n");

  std::ostringstream oss;
  oss << "sizeof(time_t): " << sizeof(time_t) << " bytes (" << (sizeof(time_t) * 8) << " bits)\n";

  // time_tの最大値を表示
  time_t maxTime = std::numeric_limits<time_t>::max();
  oss << "time_t max: " << maxTime << "\n";

  // 2038年問題の閾値を表示
  time_t y2038 = 0x7FFFFFFF; // 32bit signed intの最大値
  struct tm *tm2038 = gmtime(&y2038);
  if (tm2038) {
    oss << "2038-01-19 03:14:07(UTC) = " << y2038 << "\n";
    oss << "time_t max as date: "
      << (tm2038->tm_year + 1900) << "-"
      << std::setw(2) << std::setfill('0') << (tm2038->tm_mon + 1) << "-"
      << std::setw(2) << std::setfill('0') << tm2038->tm_mday << " "
      << std::setw(2) << std::setfill('0') << tm2038->tm_hour << ":"
      << std::setw(2) << std::setfill('0') << tm2038->tm_min << ":"
      << std::setw(2) << std::setfill('0') << tm2038->tm_sec << " (UTC)\n";
  }

  monitorIo_->send(oss.str());
  return true;
}

bool SerialCommandProcessor::opecodeEepromDump(std::vector<std::string> command) {
  uint16_t startAddress = 0;
  uint16_t datalen = 0x200; // デフォルト
//  monitorIo_->send("opecodeEepromDump\n");

  // 引数チェック
  if(command.size() > 1) {
    int tmp = parseStringToInt(command[1]);
    if(tmp < 0 || tmp > EEPROM_MAX_ADDRESS) {
      monitorIo_->send("開始アドレスが不正です\n");
      return false;
    }
    startAddress = static_cast<uint16_t>(tmp);
  }
  if(command.size() > 2) {
    int tmp = parseStringToInt(command[2]);
    if(tmp <= 0 || tmp > (EEPROM_MAX_ADDRESS - startAddress + 1)) {
      monitorIo_->send("ダンプ長が不正です\n");
      return false;
    }
    datalen = static_cast<uint16_t>(tmp);
  }

  // 16バイト単位に調整
  startAddress = (startAddress / 16) * 16;
  datalen = ((datalen + 15) / 16) * 16;

  // 上限チェック
  if(startAddress + datalen - 1 > EEPROM_MAX_ADDRESS) {
    datalen = EEPROM_MAX_ADDRESS - startAddress + 1;
  }

  if(eeprom) {
    while(datalen > 0) {
      uint16_t chunkSize = (datalen > 0x100) ? 0x100 : datalen;
      std::string dump = eeprom->dumpEepromData(startAddress, chunkSize);
      startAddress += chunkSize;
      datalen -= chunkSize;
      if(dump.empty()) {
        monitorIo_->send("EEPROMダンプ失敗\n");
        return false;
      }
      monitorIo_->send(dump);
    }
    return true;
  }
  monitorIo_->send("EEPROM未初期化\n");
  return false;
}

bool SerialCommandProcessor::opecodeI2CScan(std::vector<std::string> command) {
  // SystemControllerなどからI2CBusManagerの参照をもらう必要あり
  if(i2cBus) {
    std::vector<uint8_t> found = i2cBus->scanI2CBus();
    monitorIo_->send("Scanning...\n");
    for(auto addr : found) {
      monitorIo_->send("Found device at 0x" + toHex(addr) + "\n");
    }
    if(found.empty()) {
      monitorIo_->send("No I2C devices found.\n");
    }
    else {
      monitorIo_->send("I2C scan complete.\n");
    }
    return true;
  }
  monitorIo_->send("I2Cバス未初期化\n");
  return false;
}

std::string SerialCommandProcessor::toHex(uint8_t value) const {
  std::ostringstream oss;
  oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
  return oss.str();
}

bool SerialCommandProcessor::opecodeWiFiScan(std::vector<std::string> command) {
//  if(wiFiManager) {
  if(wiFiManager && !(wiFiManager->checkWifiScanCallback())) { // WiFiManagerが初期化されていて、スキャンコールバックが設定されていない場合
    monitorIo_->send("opecodeWiFiScan\n");
    wiFiManager->setWifiScanCallback([this]() {
      std::string result = wiFiManager->getWiFiScanResultString();
      if(monitorIo_) {
        monitorIo_->send(result);
      }
    });
    // WiFiManagerのスキャン機能を呼び出す
    wiFiManager->wifiScanRequest();
  }
  else if(wiFiManager && wiFiManager->checkWifiScanCallback()) { // スキャンコールバックが設定されている場合
    monitorIo_->send("WiFiスキャンが既に実行中です。\n");
  }
  else {
    monitorIo_->send("WiFiManagerが初期化されていません。\n");
  }
  // 仮の実装
  // 必要に応じてWiFiManager等を呼び出してください
  return true;
}

/**
 * @brief Pr設定値取得
 * 
 * @param command   コマンド引数：パラメータ番号
 * @return true 
 * @return false 
 */
bool SerialCommandProcessor::opecodeGetPr(std::vector<std::string> command)      // Pr設定値取得
{
  monitorIo_->send("opecodeGetPr\n");

  // 引数チェック
  if(command.size() == 2) {
    int tmp = parseStringToInt(command[1]);
    if(tmp < 0 || tmp > EEPROM_MAX_ADDRESS) {
      monitorIo_->send("開始アドレスが不正です\n");
      return false;
    }
  }
  else if(command.size() == 1) {
    monitorIo_->send("パラメータを指定してください。\n");
    return false;
  }
  else{
    monitorIo_->send("引数が多すぎます\n");
    return false;
  }

  int paramIndex = parseStringToInt(command[1]);
  uint8_t value = parameterManager->getParameter(paramIndex);
  std::string prValue = "Get Pr" + command[1] + " : " + std::to_string(static_cast<int>(value)) + "\n";
  monitorIo_->send(prValue);

  return true;
}

/**
 * @brief Pr設定値設定
 * 
 * @param command   コマンド引数：パラメータ番号、設定値
 * @return true 
 * @return false 
 */
bool SerialCommandProcessor::opecodeSetPr(std::vector<std::string> command)      // Pr設定値設定
{
  monitorIo_->send("opecodeSetPr\n");

  // 引数チェック
  if(command.size() == 3) {
    int tmp = parseStringToInt(command[1]);
    if(tmp < 0 || tmp > EEPROM_MAX_ADDRESS) {
      monitorIo_->send("開始アドレスが不正です\n");
      return false;
    }
  }
  else if(command.size() < 3) {
    monitorIo_->send("パラメータを指定してください。\n");
    return false;
  }
  else{
    monitorIo_->send("引数が多すぎます\n");
    return false;
  }
  
  monitorIo_->send("Set Pr" + command[1] + " : " + command[2] + "\n");

  uint8_t paramIndex = (uint8_t)parseStringToInt(command[1]);
  uint8_t value = (uint8_t)parseStringToInt(command[2]);
  if(!parameterManager->setParameter(paramIndex, value)) {
    monitorIo_->send("Pr設定値の設定に失敗しました。\n");
    return false;
  }

  return true;
}
