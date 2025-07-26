#pragma once

class IrRemoteManager {
  public:
    IrRemoteManager();

    void begin();     // IRリモコンの受信を開始する
    void update();    // IRリモコンの受信状態を更新する

  private:
};
