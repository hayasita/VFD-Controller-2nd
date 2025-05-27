// test/test_system_controller.cpp
#include "gtest/gtest.h"
#include "../src/SystemController.h"
#include "./mock/MockLogManager.h"

TEST(SystemControllerTest, LogIsWritten) {
  MockLogManager mockLogger;
  SystemController controller(&mockLogger);

  controller.doSomething();

  EXPECT_TRUE(mockLogger.containsLog("System is running"));
}
