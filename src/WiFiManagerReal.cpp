#include <M5Unified.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include "WiFiManagerReal.h"
//#include "dotserver.h"

WiFi_real::WiFi_real(){

}

/**
 * @brief WiFi.mode呼び出し
 * 
 * @param mode 
 * @return true 
 * @return false 
 */
bool WiFi_real::_mode(uint8_t mode)
{
  bool ret;
  ret = WiFi.mode((wifi_mode_t)mode);

  return ret;
}

/**
 * @brief WiFi.begin呼び出し・引数なし
 * 
 * @return true 
 * @return false 
 */
bool WiFi_real::_begin(void)
{
  return WiFi.begin();
}

/**
 * @brief WiFi.begin呼び出し・引数あり
 * 
 * @param ssid 
 * @param passphrase 
 * @return true 
 * @return false 
 */
bool WiFi_real::_begin(char* ssid, char *passphrase)
{
  return WiFi.begin(ssid, passphrase);
}


/**
 * @brief WiFi.disconnect呼び出し・引数1
 * 
 * @param wifioff 
 * @return true 
 * @return false 
 */
bool WiFi_real::_disconnect(bool wifioff)
{
  return WiFi.disconnect(wifioff);
}

/**
 * @brief WiFi.disconnect呼び出し・引数2
 * 
 * @param wifioff 
 * @param eraseap 
 * @return true 
 * @return false 
 */
bool WiFi_real::_disconnect(bool wifioff, bool eraseap)
{
  return WiFi.disconnect(wifioff, eraseap);
}

/**
 * @brief WiFi.status呼び出し
 * 
 * @return uint8_t 
 */
uint8_t WiFi_real::_status(void)
{
  return WiFi.status();
}

/**
 * @brief WiFi.softAP呼び出し
 * 
 * @param ssid 
 * @param passphrase 
 * @return true 
 * @return false 
 */
bool WiFi_real::_softAP(const char* ssid, const char* passphrase)
{
  bool ret;
  Serial.println("WiFi_real::_softAP()");
  ret = WiFi.softAP(ssid, passphrase);
  return ret;
}

/**
 * @brief WiFi.softAPdisconnect呼び出し
 * 
 * @param wifioff 
 * @return true 
 * @return false 
 */
bool WiFi_real::_softAPdisconnect(bool wifioff)
{
  bool ret;
  Serial.println("WiFi_real::_softAPdisconnect()");
  ret = WiFi.softAPdisconnect(wifioff);

  return ret;
}

/**
 * @brief APモード IPアドレスをstd::stringで返す
 * 
 * @return std::string 
 */
std::string WiFi_real::_softAPIP(void)
{
  // IPアドレスの取得
  IPAddress ip = WiFi.softAPIP();

  // IPAddressをstd::stringに変換
  String ipStr = ip.toString();
  std::string stdIpStr = ipStr.c_str();

  return stdIpStr;
}

/**
 * @brief STAモード IPアドレスをstd::stringで返す
 * 
 * @return std::string 
 */
std::string WiFi_real::_staIP(void)
{
  // IPアドレスの取得
  IPAddress ip = WiFi.localIP();

  // IPAddressをstd::stringに変換
  String ipStr = ip.toString();
  std::string stdIpStr = ipStr.c_str();

  return stdIpStr;
}

/**
 * @brief STAモード SSIDをstd::stringで返す
 * 
 * @return std::string 
 */
std::string WiFi_real::_staSSID(void)
{
  String ssidStr = WiFi.SSID();
  std::string stdSsid = ssidStr.c_str();

  return stdSsid;
}
std::string WiFi_real::_staSSID(int8_t index)
{
  String ssidStr = WiFi.SSID(index);
  std::string stdSsid = ssidStr.c_str();
//  Serial.printf("WiFi_real::_staSSID(%d) : %s\n", index, stdSsid.c_str());
  if (stdSsid.empty()) {
    Serial.println("SSID not found or index out of range.");
    return ""; // SSIDが見つからない場合は空文字列を返す
  }

  return stdSsid;
}

/**
 * @brief MDNS起動
 * 
 * @param hostName MDNS名称
 * @return true 
 * @return false 
 */
bool WiFi_real::_MDNS_begin(const char* hostName)
{
  return MDNS.begin(hostName);
}

/**
 * @brief SSID取得
 * 
 * @return std::string WebIFから渡されたSSID
 */
std::string WiFi_real::_getSsid(void)
{
//  return jsData.ssid;
  std::string ret="";
  return ret;
}

/**
 * @brief SSID Password取得
 * 
 * @return std::string WebIFから渡されたSSIDPassword
 */
std::string WiFi_real::_getSsidPass(void)
{
//  return jsData.ssidpass;
  std::string ret="";
  return ret;
}

/**
 * @brief Webから渡されたSSID、SSID Password の作業用データ消去
 * 
 */
void WiFi_real::_clearSsid(void)
{
//  jsData.ssid = "";
//  jsData.ssidpass = "";

  return;
}

/**
 * @brief 実行開始からの経過時間をmSecで返す。
 * 
 * @return unsigned long 
 */
unsigned long WiFi_real::_millis(void)
{
  return millis();
}

/**
 * @brief シリアル表示処理
 * 
 * @param data 
 * @return bool 常にtrue
 */
bool WiFi_real::_print(std::string data) {
  bool ret = true;

  String arduinoStr = String(data.c_str());
  Serial.print(arduinoStr);

  return ret;
}

/**
 * @brief WebSocket送信
 * 
 * @param sendData 
 */
void WiFi_real::_websocketSend(std::string sendData)
{
  Serial.println("websocketSend");
  Serial.println(sendData.c_str());

//  websocketSend(sendData.c_str());     // WebSocket送信

  return;
}

/**
 * @brief Webサーバ起動
 * 
 */
void WiFi_real::_startWebserver(void)
{
//  startWebserver();     //WebServer Start
  return;
}

int16_t WiFi_real::_scanNetworks(bool async)
{
  return WiFi.scanNetworks(async);
}
int16_t WiFi_real::_scanComplete(void) 
{
  return WiFi.scanComplete();
}

int32_t WiFi_real::_staRSSI(int8_t networkItem)
{
  return WiFi.RSSI(networkItem);
} // WiFi.RSSI呼び出し

int8_t WiFi_real::_encryptionType(int8_t networkItem)
{
  return WiFi.encryptionType(networkItem);
} // WiFi.encryptionType呼び出し
  
/**
 * @brief WiFiManagerのハンドルを設定
 * 
 * @param _WiFiManager
 */
void setWiFihandle(WiFiManager *_WiFiManager)
{
  WiFi.onEvent([_WiFiManager](WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
      case ARDUINO_EVENT_WIFI_READY:                // ESP32のWiFiが準備完了した。
        Serial.println("== CallBack ESP32のWiFiが準備完了した。");
        break;
      case ARDUINO_EVENT_WIFI_SCAN_DONE:            // WiFiのスキャンが完了した。
        Serial.println("== CallBack WiFiのスキャンが完了した。");
        break;
      case ARDUINO_EVENT_WIFI_STA_START:            // ステーションモードが開始された。
        Serial.println("== CallBack ステーションモードが開始された。");
        _WiFiManager->connectedCallback();
        break;
      case ARDUINO_EVENT_WIFI_STA_STOP:             // ステーションモードが停止された。
        Serial.println("== CallBack ステーションモードが停止された。");
        break;
      case ARDUINO_EVENT_WIFI_STA_CONNECTED:        // ステーションがAPに接続した。
        Serial.println("== CallBack ステーションがAPに接続した。");
        break;
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:     // ステーションがAPから切断された。
        Serial.println("== CallBack ステーションがAPから切断された。");
        break;
      case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:  // 接続中のAPの認証モードが変更された。
        Serial.println("== CallBack 接続中のAPの認証モードが変更された。");
        break;
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:           // ステーションがIPアドレスを取得した。
        Serial.println("== CallBack ステーションがIPアドレスを取得した。");
        break;
      case ARDUINO_EVENT_WIFI_STA_LOST_IP:          // ステーションがIPアドレスを失った。
        Serial.println("== CallBack ステーションがIPアドレスを失った。");
        break;

      case ARDUINO_EVENT_WIFI_AP_START:             // アクセスポイントモードが開始された。
        Serial.println("== CallBack アクセスポイントモードが開始された。");
        _WiFiManager->connectedCallback();
        break;
      case ARDUINO_EVENT_WIFI_AP_STOP:              // アクセスポイントモードが停止された。
        Serial.println("== CallBack アクセスポイントモードが停止された。");
        _WiFiManager->apStopCollBack();
        break;
      case ARDUINO_EVENT_WIFI_AP_STACONNECTED:      // クライアントがアクセスポイントに接続した。
        Serial.println("== CallBack クライアントがアクセスポイントに接続した。");
        break;
      case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:   // クライアントがアクセスポイントから切断された。
        Serial.println("== CallBack クライアントがアクセスポイントから切断された。");
        break;
      case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:     // アクセスポイントに接続しているクライアントにIPアドレスが割り当てられた。
        Serial.println("== アクセスポイントに接続しているクライアントにIPアドレスが割り当てられた。");
        break;

      default:
        break;
    }
  });

  return;
}
