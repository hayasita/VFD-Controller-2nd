#pragma once

#include "../../src/TimeManager.h"
#include <string>

class DummyTimeManager : public AbstractTimeManager {
public:
  DummyTimeManager() : AbstractTimeManager() {}

  void updateTimeZone(const std::string& tzParam) override {
    return;
  }
};