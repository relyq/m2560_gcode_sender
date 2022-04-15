#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "controllers.h"

#define BUTTON_BACK 0
#define BUTTON_YP 1
#define BUTTON_YN 2
#define BUTTON_XP 3
#define BUTTON_XN 4
#define BUTTON_ZP 5
#define BUTTON_ZN 6
#define BUTTON_ZPLUS 7
#define BUTTON_ZMINUS 8
#define BUTTON_ZMULTIPLIER 9
#define BUTTON_ZDIVIDER 10
#define BUTTON_UNUSED 11  // ??
#define BUTTON_XYPLUS 12
#define BUTTON_XYMINUS 13
#define BUTTON_XYMULTIPLIER 14
#define BUTTON_XYDIVIDER 15
#define BUTTON_COUNT (BUTTON_XYDIVIDER + 1)

double stepsXY = 10;
double stepsZ = 1;
static uint16_t feedrate = 200;
static double step_multiplierXY = 10;
static double step_multiplierZ = 1;
static const uint8_t max_multiplierZ = 10;
static const uint8_t max_multiplierXY = 100;
static const uint8_t max_stepsZ = 50;
static const uint16_t max_stepsXY = 300;
static const double min_steps = 0.01;
extern Screens currentScreen;
extern bool fileSelected;
extern uint8_t currentFile;
extern char** files;
extern QueueHandle_t qGcodeLine;

void moveController(Adafruit_GFX* tft, Adafruit_GFX_Button* buttonsMove,
                    Adafruit_GFX_Button* buttonsHome, const TSPoint p) {
  uint8_t pressed_button = 0xff;

  for (size_t i_btn = 0; i_btn < BUTTON_COUNT; i_btn++) {
    if (buttonsMove[i_btn].contains(p.x, p.y)) {
      pressed_button = i_btn;
      break;
    }
  }

  char str_rapid[10] = "G91 G1 F";
  char str_tmp[64];
  char command[128];
  char str_stepsXY[16];
  char str_stepsZ[16];

  strcpy(command, str_rapid);
  sprintf(str_tmp, "%d ", feedrate);
  strcat(command, str_tmp);

  tft->setTextColor(WHITE, BLACK);

  switch (pressed_button) {
    case BUTTON_BACK: {
      currentScreen = Screens::Home;
      if (!fileSelected) {
        drawHomeScreen(tft, buttonsHome, NULL);
        return;
      } else {
        drawHomeScreen(tft, buttonsHome, files[currentFile]);
        return;
      }
      break;
    }
    case BUTTON_YP: {
      // y+
      sprintf(str_tmp, "Y%f\n", stepsXY);
      strcat(command, str_tmp);
      xQueueSend(qGcodeLine, command, portMAX_DELAY);
      break;
    }
    case BUTTON_YN: {
      // y-
      sprintf(str_tmp, "Y-%f\n", stepsXY);
      strcat(command, str_tmp);
      xQueueSend(qGcodeLine, command, portMAX_DELAY);
      break;
    }
    case BUTTON_XP: {
      // x+
      sprintf(str_tmp, "X%f\n", stepsXY);
      strcat(command, str_tmp);
      xQueueSend(qGcodeLine, command, portMAX_DELAY);
      break;
    }
    case BUTTON_XN: {
      // x-
      sprintf(str_tmp, "X-%f\n", stepsXY);
      strcat(command, str_tmp);
      xQueueSend(qGcodeLine, command, portMAX_DELAY);
      break;
    }
    case BUTTON_ZP: {
      // z+
      sprintf(str_tmp, "Z%f\n", stepsZ);
      strcat(command, str_tmp);
      xQueueSend(qGcodeLine, command, portMAX_DELAY);
      break;
    }
    case BUTTON_ZN: {
      // z-
      sprintf(str_tmp, "Z-%f\n", stepsZ);
      strcat(command, str_tmp);
      xQueueSend(qGcodeLine, command, portMAX_DELAY);
      break;
    }
    case BUTTON_ZPLUS: {
      // z+1
      stepsZ = stepsZ + step_multiplierZ;
      if (stepsZ > step_multiplierZ * 9) {
        step_multiplierZ = step_multiplierZ * 10;
      }
      if (stepsZ > max_stepsZ) {
        stepsZ = max_stepsZ;
      }
      break;
    }
    case BUTTON_ZMINUS: {
      // z-1
      if (stepsZ > min_steps) {
        if (stepsZ <= step_multiplierZ * 1.5) {
          step_multiplierZ = step_multiplierZ / 10;
        }
        stepsZ = stepsZ - step_multiplierZ;
      }
      break;
    }
    case BUTTON_ZMULTIPLIER: {
      // zx10
      if (stepsZ * 10 >= max_stepsZ) {
        stepsZ = max_stepsZ;
        step_multiplierZ = max_multiplierZ;
      } else {
        stepsZ = stepsZ * 10;
        step_multiplierZ = step_multiplierZ * 10;
      }
      break;
    }
    case BUTTON_ZDIVIDER: {
      // z/10
      if (stepsZ / 10 > min_steps) {
        stepsZ = stepsZ / 10;
        step_multiplierZ = step_multiplierZ / 10;
      } else {
        stepsZ = min_steps;
        step_multiplierZ = min_steps;
      }
      break;
    }
    case BUTTON_UNUSED: {
      break;
    }
    case BUTTON_XYPLUS: {
      // xy+1
      stepsXY = stepsXY + step_multiplierXY;
      if (stepsXY > step_multiplierXY * 9) {
        step_multiplierXY = step_multiplierXY * 10;
      }
      if (stepsXY > max_stepsXY) {
        stepsXY = max_stepsXY;
      }
      break;
    }
    case BUTTON_XYMINUS: {
      // xy-1
      if (stepsXY > min_steps) {
        if (stepsXY <= step_multiplierXY * 1.5) {
          step_multiplierXY = step_multiplierXY / 10;
        }
        stepsXY = stepsXY - step_multiplierXY;
      }
      break;
    }
    case BUTTON_XYMULTIPLIER: {
      // xyx10
      if (stepsXY * 10 >= max_stepsXY) {
        stepsXY = max_stepsXY;
        step_multiplierXY = max_multiplierXY;
      } else {
        stepsXY = stepsXY * 10;
        step_multiplierXY = step_multiplierXY * 10;
      }
      break;
    }
    case BUTTON_XYDIVIDER: {
      // xy/10
      if (stepsXY / 10 > min_steps) {
        stepsXY = stepsXY / 10;
        step_multiplierXY = step_multiplierXY / 10;
      } else {
        stepsXY = min_steps;
        step_multiplierXY = min_steps;
      }
      break;
    }

    default:
      break;
  }

  sprintf(str_stepsXY, "%.2f ", stepsXY);
  sprintf(str_stepsZ, "%.2f ", stepsZ);
  tft->setCursor(90, 140);
  tft->print(str_stepsXY);
  tft->setCursor(220, 140);
  tft->print(str_stepsZ);

  DEBUG_PRINT("stepxy: ");
  DEBUG_PRINTLN(stepsXY);
  DEBUG_PRINT("mult_xy: ");
  DEBUG_PRINTLN(step_multiplierXY);
  DEBUG_PRINT("stepz: ");
  DEBUG_PRINTLN(stepsZ);
  DEBUG_PRINT("mult_z: ");
  DEBUG_PRINTLN(step_multiplierZ);

  vTaskDelay(pdMS_TO_TICKS(100));
}