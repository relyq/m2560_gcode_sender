#include <Arduino.h>

#define VERSION "0.9.2"

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

char** files;
uint8_t filecount;

QueueHandle_t qGcodeLine;
QueueHandle_t qGcodeFile;
QueueHandle_t qMachineStatus;

TaskHandle_t hSerialPassthrough;
TaskHandle_t hTouchscreenMenu;
TaskHandle_t hGetStatus;
TaskHandle_t hSendGcodeFile;
TaskHandle_t hSendGcodeLine;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  while (!Serial || !Serial1) {
  }

  DEBUG_PRINT(F("v"));
  DEBUG_PRINTLN(F(VERSION));

  if (!SD.begin(10)) {
    DEBUG_PRINTLN(F("sd initialization failed"));
    while (1)
      ;
  }

  root = SD.open("/");
  SD.ls(LS_A | LS_DATE | LS_SIZE | LS_R);

  SD_getFileCount(root, &filecount);
  files = calloc(filecount, sizeof(char*));

  for (size_t i = 0; i < filecount; i++) {
    files[i] = calloc(MAX_FILENAME, sizeof(char));
    SD_getFileName(root, i, files[i]);
  }

  root.close();

  qGcodeLine = xQueueCreate(1, 128);
  qGcodeFile = xQueueCreate(1, sizeof(char) * MAX_FILENAME);
  qMachineStatus = xQueueCreate(1, sizeof(struct Status));

  // priority 1
  xTaskCreate(taskTouchscreenMenu, "Touchscreen_Menu",
              configMINIMAL_STACK_SIZE * 6, NULL, configMAX_PRIORITIES - 3,
              &hTouchscreenMenu);

  // priority 2
  xTaskCreate(taskSendGcodeFile, "Send_Gcode_File",
              configMINIMAL_STACK_SIZE * 2, NULL, configMAX_PRIORITIES - 2,
              &hSendGcodeFile);
  xTaskCreate(taskSerialPassthrough, "Serial_Passthrough",
              configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 2,
              &hSerialPassthrough);

  // priority 3
  xTaskCreate(taskSendGcodeLine, "Send_Gcode_Line",
              configMINIMAL_STACK_SIZE * 2, NULL, configMAX_PRIORITIES - 1,
              &hSendGcodeLine);
  xTaskCreate(taskGetStatus, "Get_Status", configMINIMAL_STACK_SIZE, NULL,
              configMAX_PRIORITIES - 1, &hGetStatus);
  xTaskCreate(taskStackMonitor, "Stack_Monitor", configMINIMAL_STACK_SIZE, NULL,
              configMAX_PRIORITIES - 1, NULL);
}

void loop() {}
