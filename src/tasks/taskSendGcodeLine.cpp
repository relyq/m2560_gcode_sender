#include <HardwareSerial.h>

#include "gcode_tasks.h"

extern QueueHandle_t qGcodeLine;

// no quiero que se pueda enviar otra linea hasta que se complete la ultima
// como es la tarea de mayor prioridad bloquea la pantalla tactil
void taskSendGcodeLine(void* pvParameters) {
  char gcodeLine[128];
  while (1) {
    xQueueReceive(qGcodeLine, gcodeLine, portMAX_DELAY);
    DEBUG_PRINT("gcode line sent: ");
    DEBUG_PRINTLN(gcodeLine);
    Serial1.print(gcodeLine);
    while (!Serial1.find("ok")) {
      DEBUG_PRINTLN("Waiting for ok");
    }
  }
}