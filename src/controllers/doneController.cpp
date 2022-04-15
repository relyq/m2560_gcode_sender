#include <Arduino_FreeRTOS.h>
#include <queue.h>

#include "controllers.h"

extern QueueHandle_t qGcodeLine;
extern Screens currentScreen;
extern uint8_t currentFile;
extern char** files;

void doneController(Adafruit_GFX* tft, Adafruit_GFX_Button* buttonsHome) {
  xQueueSend(qGcodeLine, "G53G0X-150Y-300Z-1\n", portMAX_DELAY);
  currentScreen = Screens::Home;
  drawHomeScreen(tft, buttonsHome, files[currentFile]);
}