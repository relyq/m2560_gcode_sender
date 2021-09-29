#include "gcode_tasks.h"

extern TaskHandle_t hSerialPassthrough;
extern TaskHandle_t hTouchscreenMenu;
extern TaskHandle_t hGetStatus;
extern TaskHandle_t hSendGcodeFile;
extern TaskHandle_t hSendGcodeLine;

void taskStackMonitor(void* pvParameters) {
  UBaseType_t uxHighWaterMark;
  while (1) {
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

    Serial.print("monitor task highwater mark: ");
    Serial.println(uxHighWaterMark);

    uxHighWaterMark = uxTaskGetStackHighWaterMark(hTouchscreenMenu);

    Serial.print("touchscreen task highwater mark: ");
    Serial.println(uxHighWaterMark);

    uxHighWaterMark = uxTaskGetStackHighWaterMark(hGetStatus);

    Serial.print("status task highwater mark: ");
    Serial.println(uxHighWaterMark);

    uxHighWaterMark = uxTaskGetStackHighWaterMark(hSerialPassthrough);

    Serial.print("serial passthrough task highwater mark: ");
    Serial.println(uxHighWaterMark);

    uxHighWaterMark = uxTaskGetStackHighWaterMark(hSendGcodeFile);

    Serial.print("send file task highwater mark: ");
    Serial.println(uxHighWaterMark);

    uxHighWaterMark = uxTaskGetStackHighWaterMark(hSendGcodeLine);

    Serial.print("send line task highwater mark: ");
    Serial.println(uxHighWaterMark);

    Serial.println();

    vTaskSuspend(NULL);

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}