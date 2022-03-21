#include <HardwareSerial.h>

#include "SdFat.h"
#include "gcode_tasks.h"

extern QueueHandle_t qGcodeFile;
extern QueueHandle_t qGcodeLine;
extern SdFat SD;

void taskSendGcodeFile(void* pvParameters) {
  char filename[MAX_FILENAME];
  char gcodeLine[128];
  int n;
  while (1) {
    xQueueReceive(qGcodeFile, filename, portMAX_DELAY);
    File file = SD.open(filename, O_RDONLY);
    while ((n = file.fgets(gcodeLine, sizeof(gcodeLine), "\n")) > 0) {
      xQueueSend(qGcodeLine, gcodeLine, portMAX_DELAY);
    }
    file.close();
  }
}