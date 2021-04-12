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
#include "common_defs.h"
#include "draw_screens.h"
#include "src/Adafruit_GFX.h"     // Core graphics library
#include "src/Adafruit_TFTLCD.h"  // Hardware-specific library
#include "src/TouchScreen.h"
#include "touchscreen_config.h"

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

TouchScreen ts = TouchScreen(XP, YP, XM, YM, RX);

Screens currentScreen;  // acá guardo la pantalla activa
Screens prevScreen;     // acá guardo la pantalla anterior

void taskTouchscreenMenu(void* pvParameters);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  xTaskCreate(taskTouchscreenMenu, "Touchscreen_Menu", configMINIMAL_STACK_SIZE,
              NULL, 1, NULL);
}

void loop() {}

void taskTouchscreenMenu(void* pvParameters) {
  DEBUG_PRINT(F("v"));
  DEBUG_PRINTLN(F(VERSION));
  DEBUG_PRINT(F("TFT size is "));
  DEBUG_PRINT(tft.width());
  DEBUG_PRINT(F("x"));
  DEBUG_PRINTLN(tft.height());

  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);

  tft.fillScreen(BLACK);

  drawHomeScreen(&tft, NULL);

  while (1) {
    TSPoint p = ts.getPoint();

    if (p.z > MINPRESSURE) {
      // el modulo tactil tiene 60 puntos no dibujables en la pantalla
      TSPoint pointTemp = p;
      p.y = map(pointTemp.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.x = map(pointTemp.y, TS_MINY, TS_MAXY - 60, 0, tft.width());

      if (currentScreen != Screens::screensHome && (p.y < -1)) {
      }

      switch (currentScreen) {}
    }
    vTaskDelay(600 / portTICK_PERIOD_MS);
  }
}