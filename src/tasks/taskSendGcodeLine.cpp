#include <HardwareSerial.h>

#include "gcode_tasks.h"

extern HardwareSerial Serial;
extern QueueHandle_t qGcodeLine;
extern TaskHandle_t xTouchscreenHandle;

void taskSendGcodeLine(void* pvParameters) {
  char gcodeLine[128];
  while (1) {
    xQueueReceive(qGcodeLine, gcodeLine, portMAX_DELAY);
    vTaskSuspend(xTouchscreenHandle);
    Serial.println(gcodeLine);
    while (!Serial.find("ok")) {
    }
    vTaskResume(xTouchscreenHandle);
  }
}