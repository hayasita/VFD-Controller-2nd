#pragma once

enum class SystemEvent {
  None,
  ButtonA_Short_Pressed,
  ButtonB_Short_Pressed,
  ButtonAB_Short_Pressed,
  ButtonA_Long_Pressed,
  ButtonB_Long_Pressed,
  ButtonAB_Long_Pressed,
  WebCommand_ConnectWiFi,
  WebCommand_DisconnectWiFi,
  Scheduled_SyncTime,
  // 他にも必要に応じて追加
};
