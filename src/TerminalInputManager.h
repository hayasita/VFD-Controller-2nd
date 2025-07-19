#pragma once

#include "SystemEvent.h"

#ifdef UNIT_TEST
// ...モック定義...
class InputTerminal{}; // InputTerminalの前方宣言（実際のInputTerminalクラスは別ファイルに定義されていると仮定）
#else
// ...本来の定義...
#include <M5Unified.h>
#include "InputTerminal.h"
#endif

/*
#define BUTTON_0 41
#define BUTTON_1 0
#define BUTTON_2 9
#define BUTTON_3 10
*/
#define BUTTON_0 0
#define BUTTON_1 1

class TerminalInputManager{
  public:
    TerminalInputManager();
    ~TerminalInputManager();
    void begin(unsigned char *,unsigned char);
    virtual SystemEvent update(void);
//    uint8_t man(void);
  private:
    InputTerminal* tm;
};
  