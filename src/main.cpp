#include <Arduino.h>

#define VERSION "0.0.1"

#include "Arduino_FreeRTOS.h"
#include "DEBUG_things.h"
#include "SD_things.h"
#include "SPI.h"
#include "SdFat.h"
#include "common_defs.h"
#include "queue.h"
#include "task.h"
#include "tasks/gcode_tasks.h"

SdFat SD;

File root;

char files[5][20];
uint8_t filecount;

QueueHandle_t qGcodeLine;
QueueHandle_t qGcodeFile;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  DEBUG_PRINT(F("v"));
  DEBUG_PRINTLN(F(VERSION));

  if (!SD.begin(10)) {
    DEBUG_PRINTLN(F("sd initialization failed"));
    while (1)
      ;
  }

  root = SD.open("/");
  SD.ls(LS_R);

  SD_getFileCount(root, &filecount);
  for (uint8_t i = 0; i < filecount; i++) {
    SD_getFileName(root, i, files[i]);
  }

  root.close();

  qGcodeLine = xQueueCreate(1, 128);
  qGcodeFile = xQueueCreate(1, sizeof(char) * 16);

  xTaskCreate(taskTouchscreenMenu, "Touchscreen_Menu",
              configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
  xTaskCreate(taskSendGcodeLine, "Send_Gcode_Line",
              configMINIMAL_STACK_SIZE * 2, NULL, configMAX_PRIORITIES - 1,
              NULL);
  xTaskCreate(taskSendGcodeFile, "Send_Gcode_File",
              configMINIMAL_STACK_SIZE * 2, NULL, configMAX_PRIORITIES - 2,
              NULL);
}

void loop() {}
