#ifndef vfd_real_h
#define vfd_real_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#include <cstdint>
#include "WiFiManager.h"
//#include "jsdata.h"

class WiFi_real : public WiFi_
{
  public:
    WiFi_real();
    bool _mode(uint8_t mode) override;
    bool _begin(void) override;
    bool _begin(char* ssid, char *passphrase) override;
    bool _disconnect(bool) override;
    bool _disconnect(bool wifioff, bool eraseap) override;
    uint8_t _status(void) override;
    bool _softAP(const char* ssid, const char* passphrase) override;
    bool _softAPdisconnect(bool wifioff) override;
    std::string _softAPIP(void) override;
    std::string _staIP(void) override;
    std::string _staSSID(void) override;
    std::string _staSSID(int8_t) override;
    bool _MDNS_begin(const char* hostName) override;

    std::string _getSsid(void) override;
    std::string _getSsidPass(void) override;
    void _clearSsid(void) override;

    unsigned long _millis(void) override;
    bool _print(std::string data) override;
    void _websocketSend(std::string sendData) override;

    void _startWebserver(void) override;

    int16_t _scanNetworks(bool async) override;
    int16_t _scanComplete(void) override;
    int32_t _staRSSI(int8_t networkItem);        // WiFi.RSSI呼び出し
    int8_t _encryptionType(int8_t networkItem) override;  

};

void setWiFihandle(WiFiManager *_WiFiConnect);  // WiFiManagerのハンドルを設定

#undef GLOBAL
#endif