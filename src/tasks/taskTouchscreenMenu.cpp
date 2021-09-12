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
  static Adafruit_GFX_Button buttonsMove[7];
  static Adafruit_GFX_Button buttonsSD[2];
  static Adafruit_GFX_Button buttonsConfig[1];

  Screens currentScreen;  // acá guardo la pantalla activa
  // Screens prevScreen;     // acá guardo la pantalla anterior

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
            drawMoveScreen(&tft, buttonsMove);
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
                     (p.x >= 5 && p.x <= 315 && p.y >= 35 - 5 && p.y <= 63) &&
                     (selectedFile != 0)) {
            tft.fillCircle(300, 46, 5, WHITE);
            tft.fillCircle(300, 81, 5, BLACK);
            tft.fillCircle(300, 116, 5, BLACK);
            tft.fillCircle(300, 151, 5, BLACK);
            if (!confirmButton) {
              buttonsSD[1].drawButton();
              confirmButton = 1;
            }
            selectedFile = 0;
          } else if (filecount > 1 &&
                     (p.x >= 5 && p.x <= 315 && p.y >= 63 && p.y <= 98) &&
                     (selectedFile != 1)) {
            tft.fillCircle(300, 46, 5, BLACK);
            tft.fillCircle(300, 81, 5, WHITE);
            tft.fillCircle(300, 116, 5, BLACK);
            tft.fillCircle(300, 151, 5, BLACK);
            if (!confirmButton) {
              buttonsSD[1].drawButton();
              confirmButton = 1;
            }

            selectedFile = 1;
          } else if (filecount > 2 &&
                     (p.x >= 5 && p.x <= 315 && p.y >= 98 && p.y <= 133) &&
                     (selectedFile != 2)) {
            tft.fillCircle(300, 46, 5, BLACK);
            tft.fillCircle(300, 81, 5, BLACK);
            tft.fillCircle(300, 116, 5, WHITE);
            tft.fillCircle(300, 151, 5, BLACK);
            if (!confirmButton) {
              buttonsSD[1].drawButton();
              confirmButton = 1;
            }

            selectedFile = 2;
          } else if (filecount > 3 &&
                     (p.x >= 5 && p.x <= 315 && p.y >= 133 && p.y <= 168) &&
                     (selectedFile != 3)) {
            tft.fillCircle(300, 46, 5, BLACK);
            tft.fillCircle(300, 81, 5, BLACK);
            tft.fillCircle(300, 116, 5, BLACK);
            tft.fillCircle(300, 151, 5, WHITE);
            if (!confirmButton) {
              buttonsSD[1].drawButton();
              confirmButton = 1;
            }

            selectedFile = 3;
          }
          break;
        }
        case Screens::Move: {
          if (buttonsMove[0].contains(p.x, p.y)) {
            currentScreen = Screens::Home;
            if (!fileSelected) {
              drawHomeScreen(&tft, buttonsHome, NULL);
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            }
          } else if (buttonsMove[1].contains(p.x, p.y)) {
            // y+
            xQueueSend(qGcodeLine, "G91 G1 F200 Y10\n", portMAX_DELAY);
          } else if (buttonsMove[2].contains(p.x, p.y)) {
            // y-
            xQueueSend(qGcodeLine, "G91 G1 F200 Y-10\n", portMAX_DELAY);
          } else if (buttonsMove[3].contains(p.x, p.y)) {
            // x+
            xQueueSend(qGcodeLine, "G91 G1 F200 X10\n", portMAX_DELAY);
          } else if (buttonsMove[4].contains(p.x, p.y)) {
            // x-
            xQueueSend(qGcodeLine, "G91 G1 F200 X-10\n", portMAX_DELAY);
          } else if (buttonsMove[5].contains(p.x, p.y)) {
            // z+
            xQueueSend(qGcodeLine, "G91 G1 F200 Z10\n", portMAX_DELAY);
          } else if (buttonsMove[6].contains(p.x, p.y)) {
            // z-
            xQueueSend(qGcodeLine, "G91 G1 F200 Z-10\n", portMAX_DELAY);
          } else if (buttonsMove[7].contains(p.x, p.y)) {
            // set zero
            xQueueSend(qGcodeLine, "G10L20P1X0Y0Z0\n", portMAX_DELAY);
          }
          vTaskDelay(300 / portTICK_PERIOD_MS);

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