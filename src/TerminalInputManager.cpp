
#include "TerminalInputManager.h"

TerminalInputManager::TerminalInputManager() : tm(nullptr) {}

TerminalInputManager::~TerminalInputManager() {
    delete tm; // メモリ解放
}

void TerminalInputManager::begin(unsigned char *swLists, unsigned char swNum) {
    if (tm) {
        delete tm; // 既存のインスタンスを解放
    }
    tm = new InputTerminal(swLists, swNum); // 動的に初期化
}

SystemEvent TerminalInputManager::update(void){
  SystemEvent ret = SystemEvent::None;
  String status;
  unsigned int shortonw;
  unsigned int longonw;

  if (!tm) return SystemEvent::None;

  tm->scan();                  // Update the terminaldata
  shortonw = tm->read_s();     // Read the short key input data
  longonw = tm->read_l();      // Read the long key input data

  status = "";
  if (shortonw != 0) {
//    ret = shortonw;
    switch (shortonw) {
      case 0x01:
        status = "Short ON : BUTTON_0";
        ret = SystemEvent::ButtonA_Short_Pressed;
        break;
      case 0x02:
        status = "Short ON : BUTTON_1";
        ret = SystemEvent::ButtonB_Short_Pressed;
        break;
      case 0x03:
        status = "Short ON : BUTTON_0,1";
        ret = SystemEvent::ButtonAB_Short_Pressed;
        break;
      default:
        status = "Short Other Key.";
        ret = SystemEvent::None;
        break;
    }
    Serial.printf("%s : 0x%02x\n", status.c_str(), ret);
  }
  else if (longonw != 0) {
//    ret = longonw + 0x80;
    switch (longonw) {
      case 0x01:
        status = "Long ON : BUTTON_0";
        ret = SystemEvent::ButtonA_Long_Pressed;
        break;
      case 0x02:
        status = "Long ON : BUTTON_1";
        ret = SystemEvent::ButtonB_Long_Pressed;
        break;
      case 0x03:
        status = "Long ON : BUTTON_0,1";
        ret = SystemEvent::ButtonAB_Long_Pressed;
        break;
      default:
        status = "Long ON : Other Key";
        ret = SystemEvent::None;
        break;
    }
    Serial.printf("%s : 0x%02x\n", status.c_str(), ret);
  }
  else {}

  return ret;
}
