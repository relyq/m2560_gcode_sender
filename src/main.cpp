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
Adafruit_GFX_Button buttonsSD[1];

Screens currentScreen;  // acá guardo la pantalla activa
Screens prevScreen;     // acá guardo la pantalla anterior

void taskTouchscreenMenu(void* pvParameters);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  DEBUG_PRINT(F("v"));
  DEBUG_PRINTLN(F(VERSION));

  xTaskCreate(taskTouchscreenMenu, "Touchscreen_Menu", configMINIMAL_STACK_SIZE,
              NULL, 1, NULL);
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

  drawHomeScreen(&tft, buttonsHome);

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
        currentScreen = Screens::Home;
        drawHomeScreen(&tft, buttonsHome);
      }

      switch (currentScreen) {
        case Screens::Home:
          if (p.x >= 45 && p.x <= 275 && p.y >= 5 && p.y <= 45) {
            DEBUG_PRINT("SD screen");
            currentScreen = Screens::SD;
            drawSDScreen(&tft, NULL);
          } else if (buttonsHome[2].contains(p.x, p.y)) {
            currentScreen = Screens::Calibration;
            drawCalibrationScreen(&tft);
          }
          break;
        case Screens::SD:
          if (p.x >= 45 && p.x <= 275 && p.y >= 5 && p.y <= 45) {
            DEBUG_PRINT("Home screen");
            currentScreen = Screens::Home;
            drawHomeScreen(&tft, buttonsHome);
          }
          break;
        case Screens::Calibration:
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
    vTaskDelay(15 / portTICK_PERIOD_MS);
  }
}
