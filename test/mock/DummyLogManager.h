#pragma once

#include "../../src/LogManager.h"
#include <string>

class DummyLogManager : public LogManager {
public:
  void writeLog(const char* message) override {
    lastMessage = message;
  }
  std::string lastMessage;
};

    