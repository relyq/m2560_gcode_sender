#ifndef GCODE_TASKS_H
#define GCODE_TASKS_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "DEBUG_things.h"
#include "common_defs.h"

void taskTouchscreenMenu(void* pvParameters);
void taskSendGcodeLine(void* pvParameters);
void taskSendGcodeFile(void* pvParameters);
void taskSerialPassthrough(void* pvParameters);
void taskGetStatus(void* pvParameters);
void taskStackMonitor(void* pvParameters);

#endif