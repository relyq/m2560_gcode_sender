#include "DEBUG_things.h"
#include "common_defs.h"
#include "draw_screens.h"
#include "gcode_tasks.h"
#include "src/Adafruit_GFX.h"     // Core graphics library
#include "src/Adafruit_TFTLCD.h"  // Hardware-specific library
#include "src/TouchScreen.h"
#include "touchscreen_config.h"

extern QueueHandle_t qGcodeLine;
extern QueueHandle_t qGcodeFile;

void taskTouchscreenMenu(void* pvParameters) {
  Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

  TouchScreen ts = TouchScreen(XP, YP, XM, YM, RX);

  static Adafruit_GFX_Button buttonsHome[8];
  static Adafruit_GFX_Button buttonsMove[24];
  static Adafruit_GFX_Button buttonsSD[2];
  static Adafruit_GFX_Button buttonsConfig[1];

  Screens currentScreen;  // acá guardo la pantalla activa
  // Screens prevScreen;     // acá guardo la pantalla anterior

  static uint16_t feedrate = 200;
  static double stepsXY = 10;
  static double step_multiplierXY = 10;
  static double stepsZ = 1;
  static double step_multiplierZ = 1;
  static const uint8_t max_multiplierZ = 10;
  static const uint8_t max_multiplierXY = 100;
  static const uint8_t max_stepsZ = 50;
  static const uint16_t max_stepsXY = 300;
  static const double min_steps = 0.01;

  static uint8_t currentFile = 0xff;
  static uint8_t selectedFile = 0xff;
  extern char files[5][20];
  extern uint8_t filecount;

  bool fileSelected = 0;

  DEBUG_PRINTLN(F("Touchscreen task started."));

  DEBUG_PRINT(F("TFT size is "));
  DEBUG_PRINT(tft.width());
  DEBUG_PRINT(F("x"));
  DEBUG_PRINTLN(tft.height());

  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);

  tft.fillScreen(BLACK);

  currentScreen = Screens::Home;
  drawHomeScreen(&tft, buttonsHome, NULL);

  while (1) {
    TSPoint p = ts.getPoint();

    if (p.z > MINPRESSURE) {
      // el modulo tactil tiene 60 puntos no dibujables en la pantalla
      TSPoint pointTmp = p;

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      p.y = map(pointTmp.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.x = map(pointTmp.y, TS_MINY, TS_MAXY - 60, 0, tft.width());

      if (currentScreen != Screens::Home && (p.x > 320)) {
        currentScreen = Screens::Home;
        if (!fileSelected) {
          drawHomeScreen(&tft, buttonsHome, NULL);
        } else {
          drawHomeScreen(&tft, buttonsHome, files[currentFile]);
        }
      }

      switch (currentScreen) {
        case Screens::Home: {
          if (buttonsHome[0].contains(p.x, p.y)) {
            currentScreen = Screens::SD;
            drawSDScreen(&tft, buttonsSD, filecount, files);
            if (currentFile != 0xff) {
              tft.fillCircle(300, 42 + 28 * currentFile, 5, WHITE);
            }
          } else if (buttonsHome[1].contains(p.x, p.y)) {
            currentScreen = Screens::Move;
            drawMoveScreen(&tft, buttonsMove, stepsXY, stepsZ);
          } else if (buttonsHome[2].contains(p.x, p.y)) {
            currentScreen = Screens::Config;
            drawConfigScreen(&tft, buttonsConfig);
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
            drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            xQueueSend(qGcodeFile, files[currentFile], portMAX_DELAY);
            xQueueSend(qGcodeLine, "G28\n", portMAX_DELAY);

            tft.fillScreen(BLACK);
            tft.setTextSize(3);
            tft.setCursor((320 - (CHARACTER_WIDTH * 3 * 17)) / 2,
                          120 - (CHARACTER_HEIGHT * 3));
            tft.print("Trabajo terminado");
            currentScreen = Screens::Done;

            vTaskDelay(15 / portTICK_PERIOD_MS);
          }
          break;
        }
        case Screens::SD: {
          static bool confirmButton = 0;

          if (buttonsSD[0].contains(p.x, p.y)) {
            confirmButton = 0;
            selectedFile = currentFile;

            currentScreen = Screens::Home;
            if (!fileSelected) {
              drawHomeScreen(&tft, buttonsHome, NULL);
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            }
          } else if (confirmButton && buttonsSD[1].contains(p.x, p.y)) {
            confirmButton = 0;
            currentFile = selectedFile;
            fileSelected = 1;

            currentScreen = Screens::Home;

            drawHomeScreen(&tft, buttonsHome, files[currentFile]);

          } else if (filecount > 0 &&
                     (p.x >= 5 && p.x <= 315 && p.y >= 28 && p.y <= 56) &&
                     (selectedFile != 0)) {
            tft.fillCircle(300, 42, 5, WHITE);
            tft.fillCircle(300, 70, 5, BLACK);
            tft.fillCircle(300, 98, 5, BLACK);
            tft.fillCircle(300, 126, 5, BLACK);
            if (!confirmButton) {
              buttonsSD[1].drawButton();
              confirmButton = 1;
            }
            selectedFile = 0;
          } else if (filecount > 1 &&
                     (p.x >= 5 && p.x <= 315 && p.y >= 56 && p.y <= 84) &&
                     (selectedFile != 1)) {
            tft.fillCircle(300, 42, 5, BLACK);
            tft.fillCircle(300, 70, 5, WHITE);
            tft.fillCircle(300, 98, 5, BLACK);
            tft.fillCircle(300, 126, 5, BLACK);
            if (!confirmButton) {
              buttonsSD[1].drawButton();
              confirmButton = 1;
            }

            selectedFile = 1;
          } else if (filecount > 2 &&
                     (p.x >= 5 && p.x <= 315 && p.y >= 84 && p.y <= 112) &&
                     (selectedFile != 2)) {
            tft.fillCircle(300, 42, 5, BLACK);
            tft.fillCircle(300, 98, 5, WHITE);
            tft.fillCircle(300, 70, 5, BLACK);
            tft.fillCircle(300, 126, 5, BLACK);
            if (!confirmButton) {
              buttonsSD[1].drawButton();
              confirmButton = 1;
            }

            selectedFile = 2;
          } else if (filecount > 3 &&
                     (p.x >= 5 && p.x <= 315 && p.y >= 112 && p.y <= 140) &&
                     (selectedFile != 3)) {
            tft.fillCircle(300, 42, 5, BLACK);
            tft.fillCircle(300, 70, 5, BLACK);
            tft.fillCircle(300, 98, 5, BLACK);
            tft.fillCircle(300, 126, 5, WHITE);
            if (!confirmButton) {
              buttonsSD[1].drawButton();
              confirmButton = 1;
            }

            selectedFile = 3;
          }
          break;
        }
        case Screens::Move: {
          char str_rapid[10] = "G91 G1 F";
          char str_tmp[64];
          char command[128];
          char str_stepsXY[16];
          char str_stepsZ[16];

          strcpy(command, str_rapid);
          sprintf(str_tmp, "%d ", feedrate);
          strcat(command, str_tmp);

          tft.setTextColor(WHITE, BLACK);

          if (buttonsMove[0].contains(p.x, p.y)) {
            currentScreen = Screens::Home;
            if (!fileSelected) {
              drawHomeScreen(&tft, buttonsHome, NULL);
              break;
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
              break;
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
          tft.setCursor(90, 140);
          tft.print(str_stepsXY);
          tft.setCursor(220, 140);
          tft.print(str_stepsZ);

          DEBUG_PRINT("stepxy: ");
          DEBUG_PRINTLN(stepsXY);
          DEBUG_PRINT("mult_xy: ");
          DEBUG_PRINTLN(step_multiplierXY);
          DEBUG_PRINT("stepz: ");
          DEBUG_PRINTLN(stepsZ);
          DEBUG_PRINT("mult_z: ");
          DEBUG_PRINTLN(step_multiplierZ);

          vTaskDelay(pdMS_TO_TICKS(100));
          break;
        }
        case Screens::Config: {
          if (buttonsConfig[0].contains(p.x, p.y)) {
            currentScreen = Screens::Home;
            if (!fileSelected) {
              drawHomeScreen(&tft, buttonsHome, NULL);
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            }
          }
          break;
        }
        case Screens::Done: {
          xQueueSend(qGcodeLine, "G53G0X-184Y-249Z-1\n", portMAX_DELAY);
          currentScreen = Screens::Home;
          drawHomeScreen(&tft, buttonsHome, files[currentFile]);
        }
      }
    }
    vTaskDelay(15 / portTICK_PERIOD_MS);
  }
}