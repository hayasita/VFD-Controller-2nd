#pragma once
#include "../../src/TerminalInputManager.h"

class DummyTerminalInputManager : public TerminalInputManager {
public:
  SystemEvent update() override {
    // ダミー処理（何もしない）
    return SystemEvent();
  }
};
