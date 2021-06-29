#include <Arduino.h>

#define VERSION "0.0.1"

#include "Arduino_FreeRTOS.h"
#include "DEBUG_things.h"
#include "SD.h"
#include "SD_things.h"
#include "SPI.h"
#include "common_defs.h"
#include "queue.h"
#include "task.h"
#include "tasks/gcode_tasks.h"

File root;

char files[5][20];
uint8_t filecount;

QueueHandle_t qGcodeLine;

TaskHandle_t xTouchscreenHandle;

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

  SD_getFileCount(root, &filecount);
  DEBUG_PRINT("file count: ");
  DEBUG_PRINTLN(filecount);
  for (uint8_t i = 0; i < filecount; i++) {
    SD_getFileName(root, i, files[i]);
  }

  DEBUG_PRINT("file 1: ");
  DEBUG_PRINTLN(files[0]);
  DEBUG_PRINT("file 2: ");
  DEBUG_PRINTLN(files[1]);
  DEBUG_PRINT("file 3: ");
  DEBUG_PRINTLN(files[2]);

  qGcodeLine = xQueueCreate(1, 128);

  xTaskCreate(taskTouchscreenMenu, "Touchscreen_Menu",
              configMINIMAL_STACK_SIZE * 2, NULL, 1, &xTouchscreenHandle);
  xTaskCreate(taskSendGcodeLine, "Send_Gcode_Line",
              configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
}

void loop() {}
