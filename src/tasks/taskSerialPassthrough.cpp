#include <HardwareSerial.h>

#include "gcode_tasks.h"

void taskSerialPassthrough(void* pvParameters) {
  DEBUG_PRINTLN("Serial passthrough started.");
  while (1) {
    if (Serial1.available()) {
      Serial.println("Serial passthrough: ");
      while (Serial1.available()) {
        Serial.write(Serial1.read());
      }
    }
    vTaskDelay(60 / portTICK_PERIOD_MS);
  }
}