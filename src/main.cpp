#include <M5Unified.h>
#include "SystemController.h"

SystemController controller;

void setup() {
  Serial.begin(115200);
  controller.begin();
}

void loop() {
  controller.update();
}


