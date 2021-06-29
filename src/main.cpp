#include <Arduino.h>

#define VERSION "0.0.1"

#include "Arduino_FreeRTOS.h"
#include "DEBUG_things.h"
#include "SD.h"
#include "SPI.h"
#include "queue.h"
#include "task.h"
//#include "calibrate/calibrate.h"
#include "SD_things.h"
#include "common_defs.h"
#include "draw_screens.h"
#include "src/Adafruit_GFX.h"     // Core graphics library
#include "src/Adafruit_TFTLCD.h"  // Hardware-specific library
#include "src/TouchScreen.h"
#include "touchscreen_config.h"

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

TouchScreen ts = TouchScreen(XP, YP, XM, YM, RX);

File root;

Adafruit_GFX_Button buttonsHome[3];
Adafruit_GFX_Button buttonsMove[12];
Adafruit_GFX_Button buttonsSD[2];
Adafruit_GFX_Button buttonsConfig[2];

uint8_t currentFile = 0xff;
static uint8_t selectedFile = 0xff;
char files[5][20];
uint8_t filecount;

Screens currentScreen;  // acá guardo la pantalla activa
Screens prevScreen;     // acá guardo la pantalla anterior

QueueHandle_t qGcodeLine;

TaskHandle_t xTouchscreenHandle;

void taskTouchscreenMenu(void* pvParameters);
void taskSendGcodeLine(void* pvParameters);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  DEBUG_PRINT(F("v"));
  DEBUG_PRINTLN(F(VERSION));

  if (!SD.begin(10)) {
    DEBUG_PRINTLN(F("sd initialization failed"));
    while (1)
      ;
  }

  root = SD.open("/");

  SD_getFileCount(root, &filecount);
  DEBUG_PRINT("file count: ");
  DEBUG_PRINTLN(filecount);
  for (uint8_t i = 0; i < filecount; i++) {
    SD_getFileName(root, i, files[i]);
  }

  DEBUG_PRINT("file 1: ");
  DEBUG_PRINTLN(files[0]);
  DEBUG_PRINT("file 2: ");
  DEBUG_PRINTLN(files[1]);
  DEBUG_PRINT("file 3: ");
  DEBUG_PRINTLN(files[2]);

  qGcodeLine = xQueueCreate(1, 128);

  xTaskCreate(taskTouchscreenMenu, "Touchscreen_Menu",
              configMINIMAL_STACK_SIZE * 2, NULL, 1, &xTouchscreenHandle);
  xTaskCreate(taskSendGcodeLine, "Send_Gcode_Line",
              configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
}

void loop() {}

void taskSendGcodeLine(void* pvParameters) {
  char gcodeLine[128];
  while (1) {
    xQueueReceive(qGcodeLine, gcodeLine, portMAX_DELAY);
    vTaskSuspend(xTouchscreenHandle);
    Serial.println(gcodeLine);
    while (!Serial.find("ok")) {
    }
    vTaskResume(xTouchscreenHandle);
  }
}

void taskTouchscreenMenu(void* pvParameters) {
  DEBUG_PRINT(F("TFT size is "));
  DEBUG_PRINT(tft.width());
  DEBUG_PRINT(F("x"));
  DEBUG_PRINTLN(tft.height());

  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);

  tft.fillScreen(BLACK);

  //  drawHomeScreen(&tft, buttonsHome, "");
  drawMoveScreen(&tft, buttonsMove);
  currentScreen = Screens::Move;

  while (1) {
    TSPoint p = ts.getPoint();

    if (p.z > MINPRESSURE) {
      // el modulo tactil tiene 60 puntos no dibujables en la pantalla
      TSPoint pointTemp = p;

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      p.y = map(pointTemp.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.x = map(pointTemp.y, TS_MINY, TS_MAXY - 60, 0, tft.width());

      if (currentScreen != Screens::Home && (p.x > 320)) {
        currentScreen = Screens::Home;
        if (currentFile >= filecount) {
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
          }
          break;
        }
        case Screens::SD: {
          buttonsSD[1].initButton(&tft, 240, 220, 160, 40, WHITE, BLACK, WHITE,
                                  "Aceptar", 3);

          static bool confirmButton = 0;

          if (buttonsSD[0].contains(p.x, p.y)) {
            confirmButton = 0;
            selectedFile = currentFile;

            currentScreen = Screens::Home;
            if (currentFile >= filecount) {
              drawHomeScreen(&tft, buttonsHome, NULL);
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            }
          } else if (buttonsSD[1].contains(p.x, p.y)) {
            confirmButton = 0;
            currentFile = selectedFile;

            currentScreen = Screens::Home;
            if (currentFile >= filecount) {
              drawHomeScreen(&tft, buttonsHome, NULL);
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            }

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
          if (buttonsMove[0].contains(p.x, p.y)) {
            currentScreen = Screens::Home;
            if (currentFile >= filecount) {
              drawHomeScreen(&tft, buttonsHome, NULL);
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            }
          } else if (buttonsMove[1].contains(p.x, p.y)) {
            // y+
            xQueueSend(qGcodeLine, "G1 F200 Y10", portMAX_DELAY);
            vTaskDelay(15 / portTICK_PERIOD_MS);
          } else if (buttonsMove[2].contains(p.x, p.y)) {
            xQueueSend(qGcodeLine, "G1 F200 Y-10", portMAX_DELAY);
            vTaskDelay(15 / portTICK_PERIOD_MS);
            // y-
          } else if (buttonsMove[3].contains(p.x, p.y)) {
            xQueueSend(qGcodeLine, "G1 F200 X10", portMAX_DELAY);
            vTaskDelay(15 / portTICK_PERIOD_MS);
            // x+
          } else if (buttonsMove[4].contains(p.x, p.y)) {
            xQueueSend(qGcodeLine, "G1 F200 X-10", portMAX_DELAY);
            vTaskDelay(15 / portTICK_PERIOD_MS);
            // x-
          } else if (buttonsMove[5].contains(p.x, p.y)) {
            xQueueSend(qGcodeLine, "G1 F200 Z10", portMAX_DELAY);
            vTaskDelay(15 / portTICK_PERIOD_MS);
            // z+
          } else if (buttonsMove[6].contains(p.x, p.y)) {
            xQueueSend(qGcodeLine, "G1 F200 Z-10", portMAX_DELAY);
            vTaskDelay(15 / portTICK_PERIOD_MS);
            // z-
          }
        }
        case Screens::Config: {
          if (buttonsConfig[0].contains(p.x, p.y)) {
            currentScreen = Screens::Calibration;
            drawCalibrationScreen(&tft);
          } else if (buttonsConfig[1].contains(p.x, p.y)) {
            currentScreen = Screens::Home;
            if (currentFile >= filecount) {
              drawHomeScreen(&tft, buttonsHome, NULL);
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            }
          }
          break;
        }
        case Screens::Calibration: {
          if (p.x >= 28 && p.x <= 68 && p.y >= 16 && p.y <= 56) {
            DEBUG_PRINT(F("("));
            DEBUG_PRINT(pointTemp.x);
            DEBUG_PRINT(F(", "));
            DEBUG_PRINT(pointTemp.y);
            DEBUG_PRINTLN(F(") "));
          } else if (p.x >= 140 && p.x <= 180 && p.y >= 184 && p.y <= 224) {
            DEBUG_PRINT(F("("));
            DEBUG_PRINT(pointTemp.x);
            DEBUG_PRINT(F(", "));
            DEBUG_PRINT(pointTemp.y);
            DEBUG_PRINTLN(F(") "));
          } else if (p.x >= 252 && p.x <= 292 && p.y >= 100 && p.y <= 140) {
            DEBUG_PRINT(F("("));
            DEBUG_PRINT(pointTemp.x);
            DEBUG_PRINT(F(", "));
            DEBUG_PRINT(pointTemp.y);
            DEBUG_PRINTLN(F(") "));
          }
          break;
        }
      }
    }
    vTaskDelay(15 / portTICK_PERIOD_MS);
  }
}