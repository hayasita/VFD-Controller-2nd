
#include "IrRemoteManager.h"
#include "Config.h"

#include <IRremote.h>

IrRemoteManager::IrRemoteManager()
{
  // コンストラクタの初期化
}

void IrRemoteManager::begin()
{
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // 受信開始

  return;
}

void IrRemoteManager::update() {
  if (IrReceiver.decode()) {
    Serial.println("IR received:");
    IrReceiver.printIRResultShort(&Serial); // 受信した内容を表示
    IrReceiver.resume(); // 次の受信に備える
  }

  return;
}
