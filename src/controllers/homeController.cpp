#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "controllers.h"

extern Screens currentScreen;
extern uint8_t currentFile;
extern uint8_t filecount;
extern char** files;
extern QueueHandle_t qGcodeLine;
extern QueueHandle_t qGcodeFile;
extern double stepsXY;
extern double stepsZ;

void homeController(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsHome,
                    Adafruit_GFX_Button* buttonsSD,
                    Adafruit_GFX_Button* buttonsMove,
                    Adafruit_GFX_Button* buttonsConfig, const TSPoint p) {
  if (buttonsHome[0].contains(p.x, p.y)) {
    currentScreen = Screens::SD;
    drawSDScreen(tft, buttonsSD, filecount, files, currentFile);

  } else if (buttonsHome[1].contains(p.x, p.y)) {
    currentScreen = Screens::Move;
    drawMoveScreen(tft, buttonsMove, stepsXY, stepsZ);
  } else if (buttonsHome[2].contains(p.x, p.y)) {
    currentScreen = Screens::Config;
    drawConfigScreen(tft, buttonsConfig);
  } else if (buttonsHome[4].contains(p.x, p.y)) {
    // homing
    xQueueSend(qGcodeLine, "$H\n", portMAX_DELAY);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  } else if (buttonsHome[5].contains(p.x, p.y)) {
    // go to zero
    // xQueueSend(qGcodeLine, "G28G91X0Y0Z0\n", portMAX_DELAY); // esto
    // deberia funcionar pero esto es lo que hace bCNC
    xQueueSend(qGcodeLine, "G90\n", portMAX_DELAY);
    xQueueSend(qGcodeLine, "G0Z3\n", portMAX_DELAY);  // safe z
    xQueueSend(qGcodeLine, "G0X0Y0\n", portMAX_DELAY);
    xQueueSend(qGcodeLine, "G0Z0\n", portMAX_DELAY);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  } else if (buttonsHome[6].contains(p.x, p.y)) {
    // probe
    xQueueSend(qGcodeLine, "G38.3Z-25F30.0\n", portMAX_DELAY);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  } else if (buttonsHome[7].contains(p.x, p.y)) {
    // set zero
    xQueueSend(qGcodeLine, "G10L20P1X0Y0Z0\n", portMAX_DELAY);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  } else if (buttonsHome[3].contains(p.x, p.y)) {
    // send file
    drawHomeScreen(tft, buttonsHome, files[currentFile]);
    xQueueSend(qGcodeFile, files[currentFile], portMAX_DELAY);
    xQueueSend(qGcodeLine, "G28\n", portMAX_DELAY);

    tft->fillScreen(BLACK);
    tft->setTextSize(3);
    tft->setCursor((320 - (CHARACTER_WIDTH * 3 * 17)) / 2,
                   120 - (CHARACTER_HEIGHT * 3));
    tft->print("Trabajo terminado");
    currentScreen = Screens::Done;

    vTaskDelay(15 / portTICK_PERIOD_MS);
  }
}