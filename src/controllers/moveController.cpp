#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "controllers.h"

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
  char str_rapid[10] = "G91 G1 F";
  char str_tmp[64];
  char command[128];
  char str_stepsXY[16];
  char str_stepsZ[16];

  strcpy(command, str_rapid);
  sprintf(str_tmp, "%d ", feedrate);
  strcat(command, str_tmp);

  tft->setTextColor(WHITE, BLACK);

  if (buttonsMove[0].contains(p.x, p.y)) {
    currentScreen = Screens::Home;
    if (!fileSelected) {
      drawHomeScreen(tft, buttonsHome, NULL);
      return;
    } else {
      drawHomeScreen(tft, buttonsHome, files[currentFile]);
      return;
    }
  } else if (buttonsMove[1].contains(p.x, p.y)) {
    // y+
    sprintf(str_tmp, "Y%f\n", stepsXY);
    strcat(command, str_tmp);
    xQueueSend(qGcodeLine, command, portMAX_DELAY);
  } else if (buttonsMove[2].contains(p.x, p.y)) {
    // y-
    sprintf(str_tmp, "Y-%f\n", stepsXY);
    strcat(command, str_tmp);
    xQueueSend(qGcodeLine, command, portMAX_DELAY);
  } else if (buttonsMove[3].contains(p.x, p.y)) {
    // x+
    sprintf(str_tmp, "X%f\n", stepsXY);
    strcat(command, str_tmp);
    xQueueSend(qGcodeLine, command, portMAX_DELAY);
  } else if (buttonsMove[4].contains(p.x, p.y)) {
    // x-
    sprintf(str_tmp, "X-%f\n", stepsXY);
    strcat(command, str_tmp);
    xQueueSend(qGcodeLine, command, portMAX_DELAY);
  } else if (buttonsMove[5].contains(p.x, p.y)) {
    // z+
    sprintf(str_tmp, "Z%f\n", stepsZ);
    strcat(command, str_tmp);
    xQueueSend(qGcodeLine, command, portMAX_DELAY);
  } else if (buttonsMove[6].contains(p.x, p.y)) {
    // z-
    sprintf(str_tmp, "Z-%f\n", stepsZ);
    strcat(command, str_tmp);
    xQueueSend(qGcodeLine, command, portMAX_DELAY);
  } else if (buttonsMove[7].contains(p.x, p.y)) {
    // z+1
    stepsZ = stepsZ + step_multiplierZ;
    if (stepsZ > step_multiplierZ * 9) {
      step_multiplierZ = step_multiplierZ * 10;
    }
    if (stepsZ > max_stepsZ) {
      stepsZ = max_stepsZ;
    }

  } else if (buttonsMove[8].contains(p.x, p.y)) {
    // z-1
    if (stepsZ > min_steps) {
      if (stepsZ <= step_multiplierZ * 1.5) {
        step_multiplierZ = step_multiplierZ / 10;
      }
      stepsZ = stepsZ - step_multiplierZ;
    }

  } else if (buttonsMove[9].contains(p.x, p.y)) {
    // zx10
    if (stepsZ * 10 >= max_stepsZ) {
      stepsZ = max_stepsZ;
      step_multiplierZ = max_multiplierZ;
    } else {
      stepsZ = stepsZ * 10;
      step_multiplierZ = step_multiplierZ * 10;
    }

  } else if (buttonsMove[10].contains(p.x, p.y)) {
    // z/10
    if (stepsZ / 10 > min_steps) {
      stepsZ = stepsZ / 10;
      step_multiplierZ = step_multiplierZ / 10;
    } else {
      stepsZ = min_steps;
      step_multiplierZ = min_steps;
    }

  } else if (buttonsMove[12].contains(p.x, p.y)) {
    // xy+1
    stepsXY = stepsXY + step_multiplierXY;
    if (stepsXY > step_multiplierXY * 9) {
      step_multiplierXY = step_multiplierXY * 10;
    }
    if (stepsXY > max_stepsXY) {
      stepsXY = max_stepsXY;
    }

  } else if (buttonsMove[13].contains(p.x, p.y)) {
    // xy-1
    if (stepsXY > min_steps) {
      if (stepsXY <= step_multiplierXY * 1.5) {
        step_multiplierXY = step_multiplierXY / 10;
      }
      stepsXY = stepsXY - step_multiplierXY;
    }

  } else if (buttonsMove[14].contains(p.x, p.y)) {
    // xyx10
    if (stepsXY * 10 >= max_stepsXY) {
      stepsXY = max_stepsXY;
      step_multiplierXY = max_multiplierXY;
    } else {
      stepsXY = stepsXY * 10;
      step_multiplierXY = step_multiplierXY * 10;
    }

  } else if (buttonsMove[15].contains(p.x, p.y)) {
    // xy/10
    if (stepsXY / 10 > min_steps) {
      stepsXY = stepsXY / 10;
      step_multiplierXY = step_multiplierXY / 10;
    } else {
      stepsXY = min_steps;
      step_multiplierXY = min_steps;
    }
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