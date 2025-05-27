#pragma once

#include <M5Unified.h>
#include "InputTerminal.h"
#include "SystemEvent.h"

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
    SystemEvent update(void);
//    uint8_t man(void);
  private:
    InputTerminal* tm;
};
  