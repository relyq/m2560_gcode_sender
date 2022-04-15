#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "controllers.h"

// this could probably better be an enum
#define BUTTON_SCREEN_SD 0
#define BUTTON_SCREEN_MOVE 1
#define BUTTON_SCREEN_CONFIG 2
#define BUTTON_DONE 3
#define BUTTON_HOMING 4
#define BUTTON_GOZERO 5
#define BUTTON_PROBE 6
#define BUTTON_SETZERO 7

#define BUTTON_COUNT (BUTTON_SETZERO + 1)

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
  uint8_t pressed_button = 0xff;

  // this is not really that safe and iterating through an enum is not that much
  // better
  for (size_t i_btn = 0; i_btn < BUTTON_COUNT; i_btn++) {
    if (buttonsHome[i_btn].contains(p.x, p.y)) {
      pressed_button = i_btn;
      break;
    }
  }

  switch (pressed_button) {
    case BUTTON_SCREEN_SD: {
      currentScreen = Screens::SD;
      drawSDScreen(tft, buttonsSD, filecount, files, currentFile);
      break;
    }
    case BUTTON_SCREEN_MOVE: {
      currentScreen = Screens::Move;
      drawMoveScreen(tft, buttonsMove, stepsXY, stepsZ);
      break;
    }
    case BUTTON_SCREEN_CONFIG: {
      currentScreen = Screens::Config;
      drawConfigScreen(tft, buttonsConfig);
      break;
    }
    case BUTTON_HOMING: {
      // homing
      xQueueSend(qGcodeLine, "$H\n", portMAX_DELAY);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      break;
    }
    case BUTTON_GOZERO: {
      // go to zero
      // xQueueSend(qGcodeLine, "G28G91X0Y0Z0\n", portMAX_DELAY); // esto
      // deberia funcionar pero esto es lo que hace bCNC
      xQueueSend(qGcodeLine, "G90\n", portMAX_DELAY);
      xQueueSend(qGcodeLine, "G0Z3\n", portMAX_DELAY);  // safe z
      xQueueSend(qGcodeLine, "G0X0Y0\n", portMAX_DELAY);
      xQueueSend(qGcodeLine, "G0Z0\n", portMAX_DELAY);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      break;
    }
    case BUTTON_PROBE: {
      // probe
      xQueueSend(qGcodeLine, "G38.3Z-25F30.0\n", portMAX_DELAY);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      break;
    }
    case BUTTON_SETZERO: {
      // set zero
      xQueueSend(qGcodeLine, "G10L20P1X0Y0Z0\n", portMAX_DELAY);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      break;
    }
    case BUTTON_DONE: {
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
      break;
    }

    default: {
      break;
    }
  }
}