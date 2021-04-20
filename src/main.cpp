#include <Arduino.h>

#define VERSION "0.0.1"
#define DEBUG_ENABLED 0

#ifdef DEBUG_ENABLED
#define DEBUG_BEGIN(x) Serial.begin(x)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_BEGIN(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif

#include "Arduino_FreeRTOS.h"
//#include "calibrate/calibrate.h"
#include "common_defs.h"
#include "draw_screens.h"
#include "src/Adafruit_GFX.h"     // Core graphics library
#include "src/Adafruit_TFTLCD.h"  // Hardware-specific library
#include "src/TouchScreen.h"
#include "touchscreen_config.h"

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

TouchScreen ts = TouchScreen(XP, YP, XM, YM, RX);

Adafruit_GFX_Button buttonsHome[3];
Adafruit_GFX_Button buttonsSD[2];
Adafruit_GFX_Button buttonsConfig[2];

uint8_t currentFile = 0xff;
static uint8_t selectedFile = 0xff;

Screens currentScreen;  // acá guardo la pantalla activa
Screens prevScreen;     // acá guardo la pantalla anterior

void taskTouchscreenMenu(void* pvParameters);
int8_t SD_filecount(uint8_t* filecount, char* filenames[]);
int8_t SD_getFileName(char** buffer, uint8_t fn);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  DEBUG_PRINT(F("v"));
  DEBUG_PRINTLN(F(VERSION));

  xTaskCreate(taskTouchscreenMenu, "Touchscreen_Menu",
              configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
}

void loop() {}

void taskTouchscreenMenu(void* pvParameters) {
  DEBUG_PRINT(F("TFT size is "));
  DEBUG_PRINT(tft.width());
  DEBUG_PRINT(F("x"));
  DEBUG_PRINTLN(tft.height());

  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);

  tft.fillScreen(BLACK);

  drawHomeScreen(&tft, buttonsHome, "");

  while (1) {
    TSPoint p = ts.getPoint();

    if (p.z > MINPRESSURE) {
      // el modulo tactil tiene 60 puntos no dibujables en la pantalla
      TSPoint pointTemp = p;

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      DEBUG_PRINT(F("Unmapped p: "));
      DEBUG_PRINT(F("("));
      DEBUG_PRINT(p.x);
      DEBUG_PRINT(F(", "));
      DEBUG_PRINT(p.y);
      DEBUG_PRINT(F(", "));
      DEBUG_PRINT(p.z);
      DEBUG_PRINT(F(") "));

      p.y = map(pointTemp.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.x = map(pointTemp.y, TS_MINY, TS_MAXY - 60, 0, tft.width());

      DEBUG_PRINT(F("Mapped p: "));
      DEBUG_PRINT(F("("));
      DEBUG_PRINT(p.x);
      DEBUG_PRINT(F(", "));
      DEBUG_PRINT(p.y);
      DEBUG_PRINT(F(", "));
      DEBUG_PRINT(p.z);
      DEBUG_PRINTLN(F(") "));

      if (currentScreen != Screens::Home && (p.x > 320)) {
        char* buffer;
        SD_getFileName(&buffer, currentFile);
        currentScreen = Screens::Home;
        drawHomeScreen(&tft, buttonsHome, buffer);
      }

      switch (currentScreen) {
        case Screens::Home: {
          if (buttonsHome[0].contains(p.x, p.y)) {
            uint8_t fc;
            static char* fn[20];
            SD_filecount(&fc, fn);
            currentScreen = Screens::SD;
            drawSDScreen(&tft, buttonsSD, fc, fn);

            if (currentFile != 0xff) {
              tft.fillCircle(300, 42 + 28 * currentFile, 5, WHITE);
            }
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
          uint8_t fc;
          static char* fn[20];
          SD_filecount(&fc, fn);

          if (buttonsSD[0].contains(p.x, p.y)) {
            confirmButton = 0;
            selectedFile = currentFile;

            currentScreen = Screens::Home;
            char* currentFileName;
            SD_getFileName(&currentFileName, currentFile);
            drawHomeScreen(&tft, buttonsHome, currentFileName);
          } else if (buttonsSD[1].contains(p.x, p.y)) {
            confirmButton = 0;
            currentFile = selectedFile;

            char* currentFileName;
            SD_getFileName(&currentFileName, currentFile);
            currentScreen = Screens::Home;
            drawHomeScreen(&tft, buttonsHome, currentFileName);

          } else if (fc > 0 &&
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
          } else if (fc > 1 &&
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
          } else if (fc > 2 &&
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
          } else if (fc > 3 &&
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
        case Screens::Config: {
          if (buttonsConfig[0].contains(p.x, p.y)) {
            currentScreen = Screens::Calibration;
            drawCalibrationScreen(&tft);
          } else if (buttonsConfig[1].contains(p.x, p.y)) {
            char* currentFileName;
            SD_getFileName(&currentFileName, currentFile);
            currentScreen = Screens::Home;
            drawHomeScreen(&tft, buttonsHome, currentFileName);
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

int8_t SD_filecount(uint8_t* filecount, char* filenames[]) {
  *filecount = 4;
  filenames[0] = "aaa.gcode";
  filenames[1] = "bbb.gcode";
  filenames[2] = "ccc.gcode";
  filenames[3] = "ddd.gcode";
  return 0;
}

int8_t SD_getFileName(char** buffer, uint8_t fn) {
  switch (fn) {
    case 0: {
      *buffer = "aaa.gcode";
      break;
    }
    case 1: {
      *buffer = "bbb.gcode";
      break;
    }
    case 2: {
      *buffer = "ccc.gcode";
      break;
    }
    case 3: {
      *buffer = "ddd.gcode";
      break;
    }
    default: {
      *buffer = "";
    }
  }
  return 0;
}