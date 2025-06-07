/**
 * @file wifi_ctrl.cpp
 * @author hayasita04@gmail.com
 * @brief WiFi接続制御
 * @version 0.1
 * @date 2024-05-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include <algorithm>
#include <cstring>
#include "WiFiManager.h"
//#include "dotserver.h"

#ifndef WIFI_SCAN_RUNNING
#define WIFI_SCAN_RUNNING -1
#endif
#ifndef WIFI_AUTH_OPEN
#define WIFI_AUTH_OPEN 0
#endif

#define WL_NO_SSID_AVAIL  1
#define WL_CONNECTED      3
#define WL_DISCONNECTED   6
#define WL_NO_SHIELD      255

#define WIFI_MODE_STA     1
#define WIFI_MODE_AP      2
#define WIFI_MODE_APSTA   3

extern void sntpInit(void); // SNTP初期化 Test時にMockを使用するため、extern宣言する

const char *apSsid = "VFDController"; // AP SSID
const char *apPassword = "";
const char* MDNS_NAME = apSsid;

#define ABOTE_COUNT 60    // 終了カウント回数


WiFi_::~WiFi_(){}

WiFiManager::WiFiManager(WiFi_* pWiFi){
  pWiFi_ = pWiFi;
  init();
  wifiManuReqf = false;
  wifiScanInProgress = false; // スキャン要求フラグをクリア
  wifiScanCallback = nullptr; // スキャン完了時コールバック
}

/**
 * @brief 初期化
 * ・接続シーケンスの初期値設定・テーブル設定を行う。
 * 
 */
void WiFiManager::init(void)
{
  // WiFi接続シーケンス定義
  wifiConSts = WiFiConSts::NOCONNECTION;    // WiFi接続シーケンス初期化

  // 接続待機
  wifiSqTbl.push_back(  {WiFiConSts::NOCONNECTION       ,[&](){return noConnection();}      } );  // WiFi接続待機
  // SNTP要求自動接続
  wifiSqTbl.push_back(  {WiFiConSts::SNTPAUTO_MODESET   ,[&](){return sntpModeset();}       } );  // SNTP要求自動接続・WiFiモード設定
  wifiSqTbl.push_back(  {WiFiConSts::SNTPAUTO_STASTAR   ,[&](){return sntpStaStart();}      } );  // SNTP要求自動接続・WiFi接続
  wifiSqTbl.push_back(  {WiFiConSts::SNTPAUTO_STACON    ,[&](){return sntpStaCon();}        } );  // SNTP要求自動接続・WiFi接続完了待ち
  wifiSqTbl.push_back(  {WiFiConSts::SNTPAUTO_STACOMP   ,[&](){return sntpStaComp();}       } );  // SNTP要求自動接続・WiFi自動接続完了・SNTP初期化
  wifiSqTbl.push_back(  {WiFiConSts::SNTPAUTO_CONNECT   ,[&](){return sntpConnect();}       } );  // SNTP要求自動接続・SNTP処理完了まち
  // 手動接続
  wifiSqTbl.push_back(  {WiFiConSts::MAN_MODESET        ,[&](){return manModeset();}        } );  // 手動接続 Step1 WiFi modeset
  wifiSqTbl.push_back(  {WiFiConSts::MAN_APSTART        ,[&](){return manApStart();}        } );  // 手動接続 Step2 AP起動
  wifiSqTbl.push_back(  {WiFiConSts::MAN_APCON          ,[&](){return manApConp();}         } );  // 手動接続 Step3 AP接続成功・Server起動
  wifiSqTbl.push_back(  {WiFiConSts::MAN_MDNSSTART      ,[&](){return manMdnsStart();}      } );  // 手動接続 Step4 mDNS起動
  wifiSqTbl.push_back(  {WiFiConSts::MAN_STASTART       ,[&](){return manStaStart();}       } );  // 手動接続 Step5 WiFi STA起動
  wifiSqTbl.push_back(  {WiFiConSts::MAN_STACON         ,[&](){return manStaCon();}         } );  // 手動接続 Step6 WiFi STA接続
  wifiSqTbl.push_back(  {WiFiConSts::MAN_CONNECT        ,[&](){return manConnectComp();}    } );  // 手動接続完了
  // STAモード　SSID設定・再接続
  wifiSqTbl.push_back(  {WiFiConSts::STA_RECONNECT_DIS  ,[&](){return staReConnectDis();}   } );  // STA再接続 Step1 STA切断
  wifiSqTbl.push_back(  {WiFiConSts::STA_RECONNECT      ,[&](){return staReConnect();}      } );  // STA再接続 Step2 STA再接続
  wifiSqTbl.push_back(  {WiFiConSts::STA_RECONNECT_CON  ,[&](){return staReConnectCon();}   } );  // STA再接続 Step3 STA接続完了待ち
  // 切断
  wifiSqTbl.push_back(  {WiFiConSts::AP_DISCONNECTION   ,[&](){return apDisconnection();}   } );  // AP切断
  wifiSqTbl.push_back(  {WiFiConSts::STA_DISCONNECTION  ,[&](){return staDisconnection();}  } );  // STA切断

  wifiStaReconnectEnabled = false;    // 再接続要求を受け付けない

  return;
}

/**
 * @brief 接続要求：端子入力
 * 
 */
void WiFiManager::withItm(void)
{
  std::cout << "端子入力";
  pWiFi_->_print("-- 端子入力\n");

  wifiManuReqf = true;
  return;
}

/**
 * @brief 接続要求：無条件接続
 * 
 */
void WiFiManager::forceConnect(void) {
  if(wifiStaReconnectEnabled == 1){                   // 再接続要求を受け付ける
    ntpAutoSetSqf = SntpAutoSts::SNTPAUTO_CONNECTION; // NTP接続要求
    lastConnectionTime = pWiFi_->_millis();           // 接続した時間を保存
  }
}

/**
 * @brief 接続要求：タイマー
 * SNTP接続要求を行う。
 */
bool WiFiManager::withTimer(void)           // 接続要求：タイマー
{
  bool ret = false;
  unsigned long currentMillis = pWiFi_->_millis();

  if( (wifiConSts == WiFiConSts::NOCONNECTION)
   && (wifiStaReconnectEnabled == 1) && (reConnectInterval != 0)){  // 再接続要求を受け付ける
    if (currentMillis - lastConnectionTime >= reConnectInterval){   // 設定時間経過
      ntpAutoSetSqf = SntpAutoSts::SNTPAUTO_CONNECTION;             // NTP接続要求
      lastConnectionTime = currentMillis;
      ret = true;
    }
  }
  return ret;
}

/**
 * @brief 再接続要求を受け付けるかを設定する
 * 
 * bool enabled   再接続要求を受け付けるか
 * 
 */
void WiFiManager::setStaReconnectEnabled(uint8_t enabled)
{
  pWiFi_->_print("setStaReconnectEnabled");
  wifiStaReconnectEnabled = enabled;
  return;
}

/**
 * @brief   再接続間隔を設定する
 * 
 * @param interval  再接続間隔(時間)
 */
void WiFiManager::setReConnectInterval(uint8_t interval)
{
  reConnectInterval = (unsigned long)interval * 3600 * 1000;
//  reConnectInterval = (unsigned long)interval * 60 * 1000;      // testmode min
  return;
}

void WiFiManager::withStaReconnect(void)       // 接続要求：STA再接続要求
{
  wifiStaReconnect = true;
  return;
}

/**
 * @brief WiFi接続シーケンス取得
 * 
 * @return uint8_t WiFi接続シーケンス
 */
WiFiConSts WiFiManager::getWiFiConSts(void)
{
  return wifiConSts;
}

/**
 * @brief WiFi接続モード設定
 * 
 * @param mode    設定WiFi接続モード
 * @param nextSqf 設定成功時遷移モード
 */
void WiFiManager::wifiModeSet(uint8_t mode,WiFiConSts nextSqf)
{
  bool ret;

  ret = pWiFi_->_mode(mode);
  if(ret == true){
    pWiFi_->_print("WiFi.mode 成功\n");
    wifiConSts = nextSqf;
  }
  else{
    pWiFi_->_print("WiFi.mode 失敗\n");
  }

  return;
}

/**
 * @brief WiFi STA接続
 * 
 * @param nextSqf 処理完了後遷移シーケンス
 * @return true   接続成功
 * @return false  接続失敗
 */
bool WiFiManager::staConnect(char *ssid,char *pass,WiFiConSts nextSqf)
{
  bool ret;
  if(strlen(ssid) != 0){
    ret = pWiFi_->_begin(ssid, pass);
  }
  else{
    ret = pWiFi_->_begin();
  }

  if(ret == true){
    pWiFi_->_print("WiFi.begin 成功\n");
    wifiConSts = nextSqf;
  }
  else{
    pWiFi_->_print("WiFi.begin 失敗\n");
  }

  return ret;
}

/**
 * @brief WiFi STA 接続完了待ち
 * STA接続情報設定(モニタ用)がコメントアウト
 * 
 * @param nextSqf 処理完了後遷移シーケンス
 * @param errSqf  接続失敗時遷移シーケンス
 */
void WiFiManager::staConnectionWait(WiFiConSts nextSqf,WiFiConSts errSqf)
{
  std::string str = "{\"eventLog\":[{\"event\":140,\"data\":[0,0,0,0]}]}";

  static uint8_t staConCount = 0;
  bool connectionFale = 0;
  if(pWiFi_->_status() == WL_NO_SSID_AVAIL){
    pWiFi_->_print("-- 接続失敗 SSIDが無い。終了する。 WL_NO_SSID_AVAIL --\n");
  //  setStaStatus(STA_NO_SSID_AVAIL);  // STA接続情報設定：指定されたSSID見つからなかった
    connectionFale = 1;
  }
  else if(pWiFi_->_status() == WL_CONNECTED){
    if(wifiConSts == WiFiConSts::MAN_STACON){    // 手動接続 WiFi STA接続待ち
      pWiFi_->_print("STA IP address: ");
      pWiFi_->_print(pWiFi_->_staIP());
      pWiFi_->_print("\n");

      pWiFi_->_print("-- 接続完了 : 手動 AP & STA --\n");
//      vfdevent.setEventlogLoop(EVENT_WiFi_MANCON_STACOMP);   // 手動接続・WiFi STA接続完了
    }
    else if(wifiConSts == WiFiConSts::STA_RECONNECT_CON){
      pWiFi_->_print("-- 接続完了 : STA再接続 --\n");
    }
    staConCount = 0;
//    setStaStatus(STA_CONNECTED);  // STA接続情報設定：接続完了
    pWiFi_->_websocketSend(str);                      // WebSocket送信 再接続完了
    pWiFi_->_websocketSend(wsStaConpDataMake());      // WebSocket送信 SSID,IPアドレス

//    if (connectedCallback) connectedCallback(); // SNTP初期化コールバック関数実行

    wifiConSts = nextSqf;         // 接続完了時シーケンスに移行
  }
  else{
    pWiFi_->_print("WiFi.status() == ");
    std::string str = std::to_string(static_cast<unsigned int>(pWiFi_->_status()));
    pWiFi_->_print(str.c_str());
    pWiFi_->_print("\n");
    if(staConCount < 40){
      pWiFi_->_print(".");
      staConCount++;
    }
    else{
      pWiFi_->_print("-- 接続失敗 タイムアウト。終了する。 --\n");
//      setStaStatus(STA_TIMEOUT);  // STA接続情報設定:タイムアウト
      connectionFale = 1;
//      vfdevent.setEventlogLoop(EVENT_WiFi_STAERR_TIMEOUT);  // STA接続エラー・タイムアウト
    }
  }
  /* STA設定不備による接続失敗処理 */
  if(connectionFale == 1){
    pWiFi_->_disconnect(true);

    if(ntpAutoSetSqf == SntpAutoSts::SNTPAUTO_CONNECTION){
      pWiFi_->_print("自動接続完了\n");
      ntpAutoSetSqf = SntpAutoSts::SNTPAUTO_STANDBY;  // SSID無効で、自動接続は待機に遷移
    }

    pWiFi_->_websocketSend(str);                      // WebSocket送信 再接続完了
    pWiFi_->_websocketSend(wsStaConpDataMake());      // WebSocket送信 SSID,IPアドレス

    staConCount = 0;
    wifiConSts = errSqf;    //接続失敗時シーケンスに移行
  }

  return;
}

/**
 * @brief WebSocket STA接続完了情報作成
 * STA接続完了時に、SSIDとIPアドレスをWebSocket送信するためのデータを作成する。
 * 
 * @return std::string 
 */
std::string WiFiManager::wsStaConpDataMake(void)
{
  // StationMode SSID
  std::string html_tmp = "";
  std::string stringTmp = pWiFi_->_staSSID();
  if(stringTmp.length() != 0){
    html_tmp = html_tmp + (std::string)"{\"staSsid\" : \"" + stringTmp + (std::string)"\",\n";
  }
  else{
    html_tmp = html_tmp + (std::string)"{\"staSsid\" : \"\",\n";
  }

  // StationMode IP Adress
  stringTmp = pWiFi_->_staIP();
  if(stringTmp.length() != 0){    // todo. 0.0.0.0を検出して分岐するように修正必要。
    html_tmp = html_tmp + (std::string)"\"staIpadr\" : \"" + stringTmp + (std::string)"\"}";
  }
  else{
    html_tmp = html_tmp + (std::string)"\"staIpadr\" : \"\"}";
  }

  return html_tmp;
}

/**
 * @brief 接続中断要求
 * 接続要求を受け取ったら、中断処理にシーケンス遷移させる。
 * 自動接続要求を待機にする。
 * 
 * @param nextSqf 処理完了後遷移シーケンス
 * @return true 
 * @return false 
 */
bool WiFiManager::escape(WiFiConSts nextSqf)
{
  if(wifiManuReqf){
    wifiManuReqf = false;
    pWiFi_->_print("- 強制中断 -");
//    vfdevent.setEventlogLoop(EVENT_WiFi_MAN_DISCON);    // WiFI手動中断
    ntpAutoSetSqf = SntpAutoSts::SNTPAUTO_STANDBY;    // 自動接続待機
    wifiConSts = nextSqf;
  }

  return true;
}

/**
 * @brief WiFi接続マネージャ
 * 接続シーケンスごとの実行処理を行う。
 * 
 * @return true   処理成功
 * @return false  処理失敗・指定されたシーケンスが見つからない
 */
bool WiFiManager::manager(void)
{
  bool ret = false;

  std::vector<WiFiSqtbl>::iterator itr = std::find_if(wifiSqTbl.begin(),wifiSqTbl.end(),[&](WiFiSqtbl &c) {   // コマンド実行テーブル検索
    return(c.wifiSq == wifiConSts);
  });
  if(itr != wifiSqTbl.end()){
    ret = (*itr).execCode();    // コマンド実行
  }
  else{
    // テーブル検索失敗
    std::cout << "テーブル検索失敗";
    pWiFi_->_print("テーブル検索失敗");
  }

  return ret;
}

/**
 * @brief WiFi接続待機
 * WiFi接続要求に応じてシーケンス遷移する。
 * wifiManuReqf = true で手動接続シーケンス開始する。
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::noConnection(void)            // 接続なし
{
  bool ret = true;

  // STA再接続要求クリア
  wifiStaReconnect = false;

/*
  // websocket開始フラグクリア
  websocketConnect = false;
*/
  sntpCompleted = false;      // SNTP同期完了フラグリセット
  if(ntpAutoSetSqf == SntpAutoSts::SNTPAUTO_CONNECTION){ // NTP接続要求
    wifiMode = WIFI_MODE_STA;
    wifiConSts = WiFiConSts::SNTPAUTO_MODESET;
//    vfdevent.setEventlogLoop(EVENT_WIFI_AUTOCON_START);     // WiFi自動接続
  }

  // 手動接続要求
  if(wifiManuReqf){
    pWiFi_->_print("接続なし:手動接続要求\n");
    wifiManuReqf = false;

    timetmp = pWiFi_->_millis() - 500;
    wifiMode = WIFI_MODE_APSTA;
    wifiConSts = WiFiConSts::MAN_MODESET;    // 手動接続 WiFi modeset
  }

  return ret;
}

/**
 * @brief SNTP要求自動接続・WiFiモード設定
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::sntpModeset(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  if(tm - timetmp > 500){
    timetmp = tm;
    pWiFi_->_print("-- SNTP要求自動接続・WiFiモード設定 --\n");
    wifiModeSet(wifiMode,WiFiConSts::SNTPAUTO_STASTAR);
  }

  escape(WiFiConSts::STA_DISCONNECTION);  // スイッチ入力で中断する
  return ret;
}

/**
 * @brief SNTP要求自動接続・WiFi接続
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::sntpStaStart(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  // SNTP要求自動接続・WiFi接続
  if(tm - timetmp > 500){
    timetmp = tm;
    pWiFi_->_print("-- SNTP要求自動接続・WiFi接続 --\n");
    staConnect((char *)"",(char *)"",WiFiConSts::SNTPAUTO_STACON);   // WiFi STA接続
  }

  escape(WiFiConSts::STA_DISCONNECTION);  // スイッチ入力で中断する
  return ret;
}

/**
 * @brief SNTP要求自動接続・WiFi接続完了待ち
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::sntpStaCon(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  // WiFi STA 接続完了待ち
  if(tm - timetmp > 500){
    pWiFi_->_print("-- SNTP要求接続試行中 --\n");
    timetmp =tm;   // timmer set
    staConnectionWait(WiFiConSts::SNTPAUTO_STACOMP,WiFiConSts::NOCONNECTION);   // WiFi STA 接続完了待ち
  }
  escape(WiFiConSts::STA_DISCONNECTION);  // スイッチ入力で中断する
  return ret;
}

/**
 * @brief SNTP要求自動接続・WiFi自動接続完了・SNTP初期化
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::sntpStaComp(void)
{
  //  WiFi自動接続完了・SNTP初期化
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  // SNTP要求接続中
  pWiFi_->_print("-- SNTP要求接続完了 --\n");

  // SNTP初期化
//  sntpCont.init();
//  sntpInit();
//  if (connectedCallback) connectedCallback(); // SNTP初期化コールバック関数実行

  timetmp = tm - 500;
  sntpTimeoutChk = tm;    // SNTP Timeout Counter init
  wifiConSts = WiFiConSts::SNTPAUTO_CONNECT;

  return ret;
}
/**
 * @brief SNTP要求自動接続・SNTP処理完了まち
 * 
 * @return true SNTP処理完了
 * @return false SNTP処理中
 */
bool WiFiManager::sntpConnect(void)
{
  bool ret = false;
  unsigned long tm = pWiFi_->_millis();
  // SNTP処理完了まち
  // SNTP処理完了したらWiFiConSts::NOCONNECTIONに戻る
   if (sntpCompleted == true){   // SNTP同期完了フラグ設定？
    pWiFi_->_print("-- SNTP処理完了:WiFiCONSTS_SNTPAUTOCON2 --\n");
    sntpCompleted = false;      // SNTP同期完了フラグリセット
//      vfdevent.setEventlogLoop(EVENT_WIFI_AUTOCON_SNTP_COMP);   // SNTP処理完了
    timetmp = tm - 500;
    ntpAutoSetSqf = SntpAutoSts::SNTPAUTO_DISCONNECTION;
    wifiConSts = WiFiConSts::STA_DISCONNECTION;
    ret = true;
  }
  else if((tm - sntpTimeoutChk > (2*3600*1000)) ){  // 2時間で中止する 
    pWiFi_->_print("-- SNTP TimeOut Abote.\n");
//      vfdevent.setEventlogLoop(EVENT_WIFI_AUTOCON_SNTP_TIMEOUT);  // SNTP処理タイムアウト
    timetmp = tm - 500;
    ntpAutoSetSqf = SntpAutoSts::SNTPAUTO_DISCONNECTION;
    wifiConSts = WiFiConSts::STA_DISCONNECTION;
  }
  else if(tm - timetmp > 500){
    pWiFi_->_print(",");
    timetmp = tm;
  }
  
  escape(WiFiConSts::STA_DISCONNECTION);  // スイッチ入力で中断する
  return ret;
}


/**
 * @brief 手動接続 Step1 WiFi modeset
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::manModeset(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  if(tm - timetmp > 500){
    timetmp = tm;
    pWiFi_->_print("-- 手動接続 WiFi modeset --\n");
    wifiModeSet(wifiMode,WiFiConSts::MAN_APSTART);
  }

  escape(WiFiConSts::AP_DISCONNECTION);  // スイッチ入力で中断する
  return ret;
}

/**
 * @brief 手動接続 Step2 AP起動
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::manApStart(void)
{
  bool ret;
  unsigned long tm = pWiFi_->_millis();
  if(tm - timetmp > 500){
    timetmp = tm;
    pWiFi_->_print("-- 手動接続 WiFi AP Start --\n");
    ret = pWiFi_->_softAP(apSsid, apPassword);
    if(ret == true){
      pWiFi_->_print("WiFi.softAP 成功\n");
//      vfdevent.setEventlogLoop(EVENT_WIFI_MANCON_APSTART);      // 手動接続・WiFi APモード起動
      wifiConSts = WiFiConSts::MAN_APCON;
    }
    else{
      pWiFi_->_print("WiFi.softAP 失敗\n");
//      vfdevent.setEventlogLoop(EVENT_WIFI_MANCON_APSTARTFAIL);  // 手動接続・WiFi APモード起動失敗
    }

  }

  escape(WiFiConSts::AP_DISCONNECTION);  // スイッチ入力で中断する
  return ret;
}

/**
 * @brief 手動接続 Step3 AP接続成功・Server起動
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::manApConp(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();
  if(tm - timetmp > 100){
    timetmp = tm;   // timmer set
    // Soft AP 接続成功
    pWiFi_->_print("AP IP address: ");
    pWiFi_->_print(pWiFi_->_softAPIP());
    pWiFi_->_print("\n");
    pWiFi_->_startWebserver();     //WebServer Start
//    vfdevent.setEventlogLoop(EVENT_WIFI_MANCON_APCOMP);   // 手動接続・WiFi APモード起動完了
    wifiConSts = WiFiConSts::MAN_MDNSSTART;
  }

  return ret;
}

/**
 * @brief 手動接続 Step4 mDNS起動
 * mDNSを起動し、STA接続に遷移する。
 * STA接続なしの場合は、手動接続完了に遷移する。
 * スイッチ入力があった場合は接続中断し、AP切断に遷移する。
 * @return true 
 * @return false 
 */
bool WiFiManager::manMdnsStart(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  if(tm - timetmp > 100){
    // mDNSに名前登録 
    timetmp = tm;   // timmer set

    if (pWiFi_->_MDNS_begin(MDNS_NAME)) {
      pWiFi_->_print("MDNS responder started\n");
      pWiFi_->_print("MDNS HOST Name: ");
      pWiFi_->_print(MDNS_NAME);
      pWiFi_->_print("\n");

      if(wifiMode == WIFI_MODE_APSTA){               // SSID設定あり
        pWiFi_->_print("-- AP接続完了・STA接続要求 --\n");
//        vfdevent.setEventlogLoop(EVENT_WIFI_MANCON_STASTART);   // // 手動接続・WiFi STA起動
        wifiConSts = WiFiConSts::MAN_STASTART;
      }
      else{
        pWiFi_->_print("-- 接続完了 : AP --\n");
        wifiConSts = WiFiConSts::MAN_CONNECT;
      }
    }

  }
  escape(WiFiConSts::AP_DISCONNECTION);  // スイッチ入力で中断する

  return ret;
}

/**
 * @brief 手動接続 Step5 WiFi STA起動
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::manStaStart(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  if(tm - timetmp > 500){
    timetmp = tm;

    if(wifiMode == WIFI_MODE_APSTA){               // SSID設定あり
      pWiFi_->_print("-- STA接続要求 --\n");
      staConnect((char *)"",(char *)"",WiFiConSts::MAN_STACON);
    }
    else{
      // SSID設定無い場合は起動完了
      pWiFi_->_print("-- 接続完了 : AP --\n");
      wifiConSts = WiFiConSts::MAN_CONNECT;
    }
  }
  escape(WiFiConSts::AP_DISCONNECTION);  // スイッチ入力で中断する

  return ret;
}

/**
 * @brief 手動接続 Step6 WiFi STA接続
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::manStaCon(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  if(tm - timetmp > 500){
    timetmp = tm;   // timmer set
    staConnectionWait(WiFiConSts::MAN_CONNECT,WiFiConSts::MAN_CONNECT);   // WiFi STA 接続完了待ち
  }
  escape(WiFiConSts::AP_DISCONNECTION);  // スイッチ入力で中断する

  return ret;
}

/**
 * @brief 手動接続完了
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::manConnectComp(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

//  wifiOn = ON;  // WiFi接続ON

  // 切断処理開始
  if(wifiManuReqf){         // ボタン長ケ押し
    wifiManuReqf = false;
    pWiFi_->_print("-- AP終了開始 --\n");

//    staConCount = 0;      // 試行カウンタ
    wifiApStop = false;     // AP切断コールバックフラグ

    timetmp = tm - 500;
    wifiConSts = WiFiConSts::AP_DISCONNECTION;
  }

  // STA再接続
  if(wifiStaReconnect == true){
    wifiStaReconnect = false;
    pWiFi_->_print("-- STA再接続要求 --\n");
    pWiFi_->_print("WiFi.status():");
    std::string str = std::to_string(static_cast<unsigned int>(pWiFi_->_status()));
    pWiFi_->_print(str.c_str());
    timetmp =tm - 500;
    wifiConSts = WiFiConSts::STA_RECONNECT_DIS;  // STA再接続 STA切断
  }

  return ret;
}

/**
 * @brief STA再接続 Step1 STA切断
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::staReConnectDis(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  // STA再接続・切断
//  wifiOn = ON;  // WiFi接続ON
  static uint8_t disconCount = 0;
  if(tm - timetmp > 500){
    // WiFi STA切断・設定情報を削除
    if( (pWiFi_->_disconnect(true,true) == true) || (disconCount == 20)
     || ( pWiFi_->_status() == WL_DISCONNECTED) || (pWiFi_->_status() == WL_NO_SHIELD) ){    // STA終了
      pWiFi_->_print("STA_OFF成功\n");
//      vfdevent.setEventlogLoop(EVENT_WiFi_STA_RECON_DISCON);   // STA再接続・切断
      timetmp = tm - 500;
      wifiConSts = WiFiConSts::STA_RECONNECT;
    }
    else{
      pWiFi_->_print(".\n");
      timetmp = tm;
      disconCount++;
    }
  }

  return ret;
}

/**
 * @brief STA再接続 Step2 STA再接続
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::staReConnect(void)            // STA再接続 STA再接続
{
  bool ret = true;

  // STA再接続・接続
//  wifiOn = ON;  // WiFi接続ON
  pWiFi_->_print("-- STA再接続・接続 --\n");

  if(strcmp(pWiFi_->_getSsid().c_str(),"0") == 0){
    // SSID消去
    ret = staConnect((char *)"0",(char *)"0",WiFiConSts::STA_RECONNECT_CON);
    wifiMode = WIFI_MODE_AP;
//    vfdevent.setEventlogLoop(EVENT_WiFi_STA_RECON_SSID_DELETE);   // STA再接続・接続
    pWiFi_->_print("設定削除\n");
  }
  else{
    ret = staConnect((char *)pWiFi_->_getSsid().c_str(),(char *)pWiFi_->_getSsidPass().c_str(),WiFiConSts::STA_RECONNECT_CON);
    wifiMode = WIFI_MODE_APSTA;

//    vfdevent.setEventlogLoop(EVENT_WiFi_STA_RECON_CON_START);   // STA再接続・接続
    pWiFi_->_print("新規設定で再接続\n");
    pWiFi_->_print("getSsid : ");
    pWiFi_->_print(pWiFi_->_getSsid().c_str());
    pWiFi_->_print("getSsidPass : ");
    pWiFi_->_print(pWiFi_->_getSsidPass().c_str());
  }
  pWiFi_->_clearSsid();             // Webから送信されたSSID,Passwordをクリア

  timetmp = pWiFi_->_millis();;     // timmer set
  wifiConSts = WiFiConSts::STA_RECONNECT_CON;

  return ret;
}

/**
 * @brief STA再接続 Step3 STA接続完了待ち
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::staReConnectCon(void)         // STA再接続 STA接続待ち
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  // STA再接続・接続完了待ち
//  wifiOn = ON;  // WiFi接続ON
  if(tm - timetmp > 500){
    timetmp = tm;   // timmer set
    staConnectionWait(WiFiConSts::MAN_CONNECT,WiFiConSts::MAN_CONNECT);   // WiFi STA 接続完了待ち
  }
  escape(WiFiConSts::AP_DISCONNECTION);  // ボタン入力で中断する

  return ret;
}

/**
 * @brief AP切断
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::apDisconnection(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();
  static uint8_t connectCount;

  // 切断
  if(tm - timetmp > 500){
    
//    pWiFi_->_print("-- AP切断 --\n");

    if((pWiFi_->_softAPdisconnect(true) == true) || (wifiApStop == true) || (connectCount > ABOTE_COUNT)){     // AP終了
      pWiFi_->_print("AP_OFF成功\n");
      pWiFi_->_print("staConCount : ");
      std::string str = std::to_string(static_cast<unsigned int>(connectCount));
      pWiFi_->_print(str.c_str());
      pWiFi_->_print("\n");

      if(wifiMode == WIFI_MODE_APSTA){
        timetmp = tm - 500;
        pWiFi_->_print("-- STA終了開始 --\n");
        wifiConSts = WiFiConSts::STA_DISCONNECTION;
      }
      else{
        wifiConSts = WiFiConSts::NOCONNECTION;  // STA無いので切断完了
      }
    }
    else{
//        Serial.println("AP_OFF失敗");
      connectCount ++;
      pWiFi_->_print(".");
      timetmp = tm;
//        Serial.print("** wifiStatus : ");
//        Serial.println(WiFi.status());
    }
  }

  return ret;
}

/**
 * @brief STA切断
 * 
 * @return true 
 * @return false 
 */
bool WiFiManager::staDisconnection(void)
{
  bool ret = true;
  unsigned long tm = pWiFi_->_millis();

  if(tm - timetmp > 500){
    timetmp = tm;

    if((pWiFi_->_disconnect(true) == true) || (pWiFi_->_status() == WL_NO_SHIELD)){    // STA終了
      pWiFi_->_print("STA_OFF成功\n");
      pWiFi_->_print("// wifiStatus : ");
      std::string str = std::to_string(static_cast<unsigned int>(pWiFi_->_status()));
      pWiFi_->_print(str.c_str());
//      vfdevent.setEventlogLoop(EVENT_WiFi_DISCON);    // WiFI切断
      wifiConSts = WiFiConSts::NOCONNECTION;
      lastConnectionTime = pWiFi_->_millis();           // 接続した時間を保存
    }
    else{
      pWiFi_->_print(",");
  //    Serial.print(WiFi.status());
    }

  }

  return ret;
}

/**
 * @brief CallBack ステーションがAPから切断された。
 * 
 */
void WiFiManager::staDisconCollBack(void)
{

  return;
}

/**
 * @brief CallBack アクセスポイントモードが停止された
 * アクセスポイントモード停止のコールバックから呼び出される。
 */
void WiFiManager::apStopCollBack(void)
{
//  pWiFi_->_print("\n== CallBack ESP32 soft-AP stop.");
  wifiApStop = true;
//  if (disconnectedCallback) disconnectedCallback(); // AP停止コールバック関数実行

  return;
}

/**
 * @brief WiFi接続状態取得
 * この関数は、WiFiManagerの接続状態を確認するために使用される。
 * WiFiManagerの内部状態をロックし、接続状態を取得する。
 */
bool WiFiManager::isConnected() const {
  std::lock_guard<std::mutex> lock(mutex);
  return pWiFi_->_status() == WL_CONNECTED;
}

/**
 * @brief WiFiManager 切断処理
 * WiFiManagerの切断処理を行う。
 * WiFi接続を切断し、接続状態を更新する。
 * 切断時コールバック関数を呼び出す。
 */
void WiFiManager::disconnect() {
  std::lock_guard<std::mutex> lock(mutex);
  pWiFi_->_disconnect(true);
  wasConnected = false;
  if (disconnectedCallback) disconnectedCallback();
}

/**
 * @brief WiFiManager 更新処理
 * WiFiManagerの更新処理を行う。
 * タイマー接続要求処理とWiFi接続マネージャを呼び出す。
 * 接続状態の変化があった場合は、コールバック関数を呼び出す。
 */
void WiFiManager::update() {
  std::lock_guard<std::mutex> lock(mutex);

  withTimer();  // 接続要求：タイマー
  manager();    // WiFi接続マネージャ

  bool nowConnected = pWiFi_->_status() == WL_CONNECTED;
  if (nowConnected && !wasConnected) {
    wasConnected = true;
    if (connectedCallback) connectedCallback();
  } else if (!nowConnected && wasConnected) {
    wasConnected = false;
    if (disconnectedCallback) disconnectedCallback();
  }

  // WiFiスキャン結果がある場合はコールバックを呼び出す
  if(wifiScanResult()) {
    wifiScanCallback(resultScanSsid); // スキャン結果をコールバック
    wifiScanCallback = nullptr;       // 一度だけ呼ぶ場合は解除
  }

}

/**
 * @brief WiFi接続時コールバック関数設定
 * @param callback  接続時コールバック関数
 */
void WiFiManager::onConnected(std::function<void()> callback) {
//  std::lock_guard<std::mutex> lock(mutex);
  connectedCallback = callback;
}

/**
 * @brief WiFi切断時コールバック関数設定
 * @param callback  切断時コールバック関数
 */
void WiFiManager::onDisconnected(std::function<void()> callback) {
//  std::lock_guard<std::mutex> lock(mutex);
  disconnectedCallback = callback;
}

#ifdef DELETE
/**
 * @brief WiFi STAモードの接続先検索
 * WiFiネットワークのスキャンを行う
 * 
 */
void WiFiManager::wifiScanSta(void)
{
  static uint8_t getwifiListsqf = 0;      // WiFiリスト取得シーケンスフラグ
  uint8_t getwifiStaListreq = 1;         // WiFiリスト取得要求フラグ  TestTest

  pWiFi_->_print("wifiScanSta\r\n");

  if(getwifiStaListreq == 1){             // WiFiリスト取得要求
    int16_t ssidNum;                      // WiFiリスト取得数
//    if(getwifiListsqf == 0){    
//        vfdevent.setEventlogLoop(EVENT_WiFi_SSIDSCAN_START);
      getwifiListsqf = 1;                 // WiFiリスト取得シーケンスフラグ：スキャン開始
//    }
//    else if(getwifiListsqf == 1){         // WiFiリスト取得シーケンスフラグ：スキャン開始
      pWiFi_->_print("scan done\r\n");
      pWiFi_->_mode(WIFI_MODE_APSTA);  // WiFiモードをAPSTAに設定
      ssidNum = pWiFi_->_scanNetworks(true);  // WiFiスキャン開始
      getwifiListsqf = 2;                 // WiFiリスト取得シーケンスフラグ：スキャン結果取得
//    }
//    else if(getwifiListsqf == 2){         // WiFiリスト取得シーケンスフラグ：スキャン結果取得
    do{
      ssidNum = pWiFi_->_scanComplete();      // WiFiスキャン完了確認
      if(ssidNum == WIFI_SCAN_RUNNING){   // WiFiスキャン中
        pWiFi_->_print("WIFI_SCAN_RUNNING\n");
      }
    }while(ssidNum == WIFI_SCAN_RUNNING);  // WiFiスキャン完了まで待機
//    else if(ssidNum == WiFi_SCAN_FAILED){
          // 失敗
//    }
//      else{                                // WiFiスキャン完了  
//          vfdevent.setEventlogLoop(EVENT_WiFi_SSIDSCAN_COMP);     // WiFi SSID 検索完了
        #define SSIDLIMIT 30              // SSIDリスト取得数制限 最大30
        std::string ssid_rssi_str[SSIDLIMIT];
        std::string ssid_str[SSIDLIMIT];
//        String str = "\"stationList\":[\n";
        std::string str = "{\"stationList\":[\n";
        if (ssidNum == 0) {
          pWiFi_->_print("no networks found\n");
        } else {
//          pWiFi_->_print("%d networks found\r\n\r\n", ssidNum);
          std::string ssidNumStr = std::to_string(ssidNum);
          pWiFi_->_print(ssidNumStr.c_str());
          pWiFi_->_print("networks found\r\n\r\n");
          if (ssidNum > SSIDLIMIT) ssidNum = SSIDLIMIT;   // SSIDリスト取得数制限
          for (int8_t i = 0; i < ssidNum; ++i) {             // WiFiリスト取得Loop
//          for (int8_t i = 0; i < 2; ++i) {             // WiFiリスト取得Loop
//            ssid_str[i] = WiFi.SSID(i);
            ssid_str[i] = pWiFi_->_staSSID(i);
//            std::string wifi_auth_open = ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            std::string wifi_auth_open = ((pWiFi_->_encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
//            ssid_rssi_str[i] = ssid_str[i] + " (" + WiFi.RSSI(i) + "dBm)" + wifi_auth_open;
            ssid_rssi_str[i] = ssid_str[i] /*+ " (" + WiFi.RSSI(i) + "dBm)"*/ + wifi_auth_open;
            if(i != 0){   // 2件目以降はカンマを付加
              str = str + ",\n";
            }
            str = str + "{\"ID\":\"" + ssid_str[i] + "\",\"TITLE\":\"" + ssid_rssi_str[i] + "\"}";

            std::string numStr = std::to_string(i);
            pWiFi_->_print(numStr.c_str());
            pWiFi_->_print(":");
            pWiFi_->_print(ssid_str[i]);
            pWiFi_->_print(ssid_rssi_str[i].c_str());
            pWiFi_->_print("\r\n");
          }
//          str = str + "\n]";
          str = str + "\n]}";
//            websocketDataSend.wifiStaList = str;
//            websocketDataSend.wifiStaListSend = ON;
          pWiFi_->_print(str);
          pWiFi_->_print("\n\n");
//          websocketSend(str);     //************** */
          getwifiStaListreq = 0;
        }
//      }
    }
//  }
//  else{
//    getwifiListsqf = 0;
//  }

  return;
}
#endif
/**
 * @brief WiFiスキャンコールバック関数設定
 * WiFiスキャン結果を受け取るコールバック関数を設定する。
 */
void WiFiManager::setWifiScanCallback(std::function<void(std::string)> callback) {
  std::lock_guard<std::mutex> lock(mutex);
  wifiScanCallback = callback;
}

bool WiFiManager::checkWifiScanCallback(void) const
{
  std::lock_guard<std::mutex> lock(mutex);
  return (wifiScanCallback != nullptr);
}

/**
 * @brief WiFiスキャン要求
 * WiFiネットワークのスキャンを要求する。
 */
void WiFiManager::wifiScanRequest(void)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (pWiFi_ && (wifiScanCallback != nullptr)) {
    pWiFi_->_scanNetworks(true); // WiFiスキャン要求
    wifiScanInProgress = true; // スキャン要求フラグを立てる
    pWiFi_->_print("WiFiスキャン要求\n");
  }
  else{
    std::cout << "pWiFiManager is nullptr" << std::endl;
  }
}

/**
 * @brief WiFiスキャン結果取得
 * WiFiスキャンが完了したかどうかを確認し、結果を取得する。
 */
bool WiFiManager::wifiScanResult(void)
{
//  std::lock_guard<std::mutex> lock(mutex);
  if (pWiFi_ && wifiScanInProgress) {
    int16_t ssidNum = pWiFi_->_scanComplete(); // スキャン完了確認
    if (ssidNum != WIFI_SCAN_RUNNING) {
      WifiScanResultData resultData; // スキャン結果データ構造体
      resultScanSsid = "Scan Results:" + std::to_string(ssidNum) + ":\n";
      wifiScanInProgress = false; // スキャン要求フラグをクリア
      wifiScanResultData.clear(); // スキャン結果データをクリア
      
      // スキャン結果を保存
      for (int i = 0; i < ssidNum; ++i) {
        resultData.ssid = pWiFi_->_staSSID(i); // SSIDを保存
        resultData.rssi = pWiFi_->_staRSSI(i); // RSSIを保存
        resultData.encryptionType = pWiFi_->_encryptionType(i); // 暗号化タイプを保存
//        resultData.channel = pWiFi_->_staChannel(i); // チャンネルを保存
        wifiScanResultData.push_back(resultData); // スキャン結果データに追加

        resultScanSsid += resultData.ssid + "(" + std::to_string(resultData.rssi) + "dBm):" + ((resultData.encryptionType == WIFI_AUTH_OPEN)?" ":"*") +"\n";
      }

      return true; // スキャンが完了した
    }
  }
  return false; // スキャンがまだ完了していない
}

/**
 * @brief WiFiスキャン結果を文字列形式で取得
 * スキャン結果を文字列形式で取得する。
 * 
 * @return std::string スキャン結果の文字列
 */
std::string WiFiManager::getWiFiScanResultString(void)
{
  std::string result;
  for (const auto& data : wifiScanResultData) {
    result += "SSID: " + data.ssid
           + ", Encryption: " + std::to_string(static_cast<int>(data.encryptionType))
           + ", RSSI: " + std::to_string(data.rssi) + "\n";
  }

  return result;
}

/**
 * @brief WiFiスキャン結果をJSON形式で取得
 * スキャン結果をJSON形式の文字列として取得する。
 * 
 * @return std::string スキャン結果のJSON文字列
 */
std::string WiFiManager::getWiFiScanResultJson(void)
{
  std::string result = "{\"stationList\":[\n";
  for (size_t i = 0; i < wifiScanResultData.size(); ++i) {
    const auto& data = wifiScanResultData[i]; 
    std::string ssid_rssi_str = data.ssid + " (" +std::to_string(data.rssi) + "dBm)" + ((data.encryptionType == WIFI_AUTH_OPEN)?" ":"*");
    if (i != 0){   // 2件目以降はカンマを付加
      result = result + ",\n";
    }
    result = result + "{\"ID\":\"" + data.ssid + "\",\"TITLE\":\"" + ssid_rssi_str + "\"}";
  }
  result = result + "\n]}";
  return result;
}
