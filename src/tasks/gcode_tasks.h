#ifndef GCODE_TASKS_H
#define GCODE_TASKS_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>

void taskTouchscreenMenu(void* pvParameters);
void taskSendGcodeLine(void* pvParameters);

#endif