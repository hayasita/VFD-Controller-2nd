/**
 * @file wifi_ctrl.h
 * @author hayasita04@gmail.com
 * @brief WiFi接続制御
 * @version 0.1
 * @date 2024-06-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef wifi_ctrl_h
#define wifi_ctrl_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <cstdint>
#include <vector>
#include <functional>
#include <mutex>

/**
 * @brief WiFi制御クラス
 * 
 */
class WiFi_{
  public:
    virtual ~WiFi_(void) = 0;   // 純粋仮想デストラクタ
    virtual bool _mode(uint8_t mode) = 0;
    virtual bool _begin(void) = 0;
    virtual bool _begin(char* ssid, char *passphrase) = 0;
    virtual bool _disconnect(bool) = 0;
    virtual bool _disconnect(bool wifioff, bool eraseap) = 0;
    virtual uint8_t _status(void) = 0;
    virtual bool _softAP(const char* ssid, const char* passphrase) = 0;
    virtual bool _softAPdisconnect(bool wifioff) = 0;
    virtual std::string _softAPIP(void) = 0;
    virtual std::string _staIP(void) = 0;
    virtual std::string _staSSID(void) = 0;
    virtual bool _MDNS_begin(const char* hostName) = 0;

    virtual std::string _getSsid(void) = 0;
    virtual std::string _getSsidPass(void) = 0;
    virtual void _clearSsid(void) = 0;

    virtual unsigned long _millis(void) = 0;
    virtual bool _print(std::string data) = 0;
    virtual void _websocketSend(std::string sendData) = 0;

    virtual void _startWebserver(void) = 0;
};

/**
 * @brief SNTP自動接続ステータス
 * 
 */
enum class SntpAutoSts{
  SNTPAUTO_STANDBY = 0,       // 待機
  SNTPAUTO_CONNECTION,        // 接続
  SNTPAUTO_DISCONNECTION      // 切断
};

/**
 * @brief WiFi接続ステータス
 * 
 */
enum class WiFiConSts{
  NOCONNECTION = 0,      // 未接続

  SNTPAUTO_MODESET,      // SNTP自動接続・WiFiモード設定
  SNTPAUTO_STASTAR,      // SNTP自動接続・WiFi接続
  SNTPAUTO_STACON,       // SNTP自動接続・WiFi接続完了待ち
  SNTPAUTO_STACOMP,      // SNTP自動接続・WiFi自動接続完了・SNTP初期化
  SNTPAUTO_CONNECT,      // SNTP自動接続・SNTP処理完了まち

  MAN_MODESET,           // 手動接続 Step1 WiFi modes
  MAN_APSTART,           // 手動接続 Step2 AP起動
  MAN_APCON,             // 手動接続 Step3 AP接続成功
  MAN_MDNSSTART,         // 手動接続 Step4 mDNS起動
  MAN_STASTART,          // 手動接続 Step5 WiFi STA起動
  MAN_STACON,            // 手動接続 Step6 WiFi STA接続
  MAN_CONNECT,           // 手動接続完了

  STA_RECONNECT_DIS,     // STA再接続 Step1 STA切断
  STA_RECONNECT,         // STA再接続 Step2 STA再接続
  STA_RECONNECT_CON,     // STA再接続 Step3 STA接続完了待ち

  AP_DISCONNECTION,      // AP切断
  STA_DISCONNECTION      // STA切断
};

/**
 * @brief WiFi接続シーケンステーブル
 * 
 */
class WiFiSqtbl{
  public:
    WiFiConSts wifiSq;                  // WiFi接続シーケンス
    std::function<bool()> execCode;     // コード実行処理
};

/**
 * @brief WiFi接続シーケンス
 * 
 */
class WiFiManager{
  friend class WiFiCtrlTest;        // フレンドテストの定義
  public:
    WiFiManager(WiFi_*);            // コンストラクタ
    bool manager(void);             // WiFiシーケンスマネージャ
    void withItm(void);             // 接続要求：端子入力
    bool withTimer(void);           // 接続要求：タイマー
    void withStaReconnect(void);    // 接続要求：STA再接続要求
    void forceConnect(void);        // 接続要求：無条件接続

    void setStaReconnectEnabled(uint8_t enabled); // 再接続要求を受け付けるかを設定する
    void setReConnectInterval(uint8_t interval);  // 再接続間隔を設定する

    WiFiConSts getWiFiConSts(void);     // WiFi接続シーケンス取得

    // WiFi イベントハンドル
    void staDisconCollBack(void);       // CallBack ステーションがAPから切断された。
    void apStopCollBack(void);          // CallBack アクセスポイントモードが停止された。

    bool staDisconnection(void);        // STA切断

    void update(); // 状態確認用
    bool isConnected() const;
    void disconnect();
    void onConnected(std::function<void()> callback);
    void onDisconnected(std::function<void()> callback);

    bool sntpCompleted;                 // SNTP同期完了フラグ
  private:
    WiFi_ *pWiFi_  = nullptr;           // WiFi制御用ポインタ
    unsigned long timetmp;              // 処理経過時間tmp

    bool wifiManuReqf;                  // WiFI 手動要求
    bool wifiStaReconnect;              // WiFi STA再接続要求
    SntpAutoSts ntpAutoSetSqf;          // NTP AutoConnection シーケンス

    uint8_t wifiStaReconnectEnabled;    // 再接続要求を受け付けるか

    std::vector<WiFiSqtbl> wifiSqTbl;   // WiFi接続シーケンステーブル
    WiFiConSts wifiConSts;                 // WiFi接続シーケンス
    uint8_t wifiMode;                   // WiFi接続モード     // 用途確認

    unsigned long lastConnectionTime;   // 最終接続時間
    unsigned long reConnectInterval;    // 再接続間隔

    void init(void);                    // 初期化処理

    // 共通処理
    void wifiModeSet(uint8_t mode,WiFiConSts nextSqf);              // WiFi接続モード設定
    bool staConnect(char *ssid,char *pass,WiFiConSts nextSqf);      // WiFi STA接続
    void staConnectionWait(WiFiConSts nextSqf,WiFiConSts errSqf);   // WiFi STA 接続完了待ち
    bool escape(WiFiConSts nextSqf);                                // 接続中断要求

    bool wifiApStop;                    // AP切断コールバックフラグ
    unsigned long sntpTimeoutChk;       // SNTP接続タイムアウトチェック

    std::string wsStaConpDataMake(void);     // WebSocket STA接続完了情報作成
    
    // WiFi接続シーケンス処理
    bool noConnection(void);            // WiFi接続待機

    bool sntpModeset(void);             // SNTP要求自動接続・WiFiモード設定
    bool sntpStaStart(void);            // SNTP要求自動接続・WiFi接続
    bool sntpStaCon(void);              // SNTP要求自動接続・WiFi接続完了待ち
    bool sntpStaComp(void);             // SNTP要求自動接続・WiFi自動接続完了・SNTP初期化
    bool sntpConnect(void);             // SNTP要求自動接続・SNTP処理完了まち

    bool manModeset(void);              // 手動接続 Step1 WiFi modeset
    bool manApStart(void);              // 手動接続 Step2 AP起動
    bool manApConp(void);               // 手動接続 Step3 AP接続成功・Server起動
    bool manMdnsStart(void);            // 手動接続 Step4 mDNS起動
    bool manStaStart(void);             // 手動接続 Step5 WiFi STA起動
    bool manStaCon(void);               // 手動接続 Step6 WiFi STA接続
    bool manConnectComp(void);          // 手動接続完了

    bool staReConnectDis(void);         // STA再接続 Step1 STA切断
    bool staReConnect(void);            // STA再接続 Step2 STA再接続
    bool staReConnectCon(void);         // STA再接続 Step3 STA接続完了待ち

    bool apDisconnection(void);         // AP切断
//    bool staDisconnection(void);        // STA切断

    mutable std::mutex mutex;
    std::function<void()> connectedCallback;
    std::function<void()> disconnectedCallback;
    bool wasConnected = false;
};

#undef GLOBAL
#endif
