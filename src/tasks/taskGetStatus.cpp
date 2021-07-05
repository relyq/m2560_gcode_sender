#include "common_defs.h"
#include "gcode_tasks.h"

extern TaskHandle_t hSerialPassthrough;
extern QueueHandle_t qMachineStatus;

void taskGetStatus(void* pvParameters) {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  struct Status position;
  vTaskSuspend(hSerialPassthrough);
  DEBUG_PRINTLN("Status task started.");
  while (1) {
    char status[72];
    char* saveptr;
    uint8_t i = 0;

    Serial1.write('?');
    while (Serial1.available()) {
      status[i] = Serial1.read();
      if (status[i] == '>') {
        status[i + 1] = '\0';
        break;
      }
      i++;
    }

    if (status[0] != '<') {
      strcpy(&status[0], &status[2]);
    }

    position.state = strtok_r(status, "\n<|:,>", &saveptr);
    strtok_r(NULL, " \n<|:,>", &saveptr);
    position.x = strtok_r(NULL, "\n<|:,>", &saveptr);
    position.y = strtok_r(NULL, "\n<|:,>", &saveptr);
    position.z = strtok_r(NULL, "\n<|:,>", &saveptr);

    xQueueSend(qMachineStatus, &position, portMAX_DELAY);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}