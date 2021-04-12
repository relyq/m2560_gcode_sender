#include <Arduino.h>

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
#include "src/Adafruit_GFX.h"     // Core graphics library
#include "src/Adafruit_TFTLCD.h"  // Hardware-specific library
#include "src/TouchScreen.h"

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3  // Chip Select goes to Analog 3
#define LCD_CD A2  // Command/Data goes to Analog 2
#define LCD_WR A1  // LCD Write goes to Analog 1
#define LCD_RD A0  // LCD Read goes to Analog 0
// optional
#define LCD_RESET A4  // Can alternately just connect to Arduino's reset pin

// touchscreen
#define YP A3  // must be an analog pin // LCD CS
#define XM A2  // must be an analog pin // LCD RS - COMMAND/DATA
#define YM 9   // can be a digital pin  // LCD D1
#define XP 8   // can be a digital pin  // LCD D0
#define RX 10  // resistance between x+ and x-

// puntos maximos y minimos de la pantalla tactil, contando el espacio
// no-dibujable
#define TS_MINX 120
#define TS_MINY 75
#define TS_MAXX 900
#define TS_MAXY 950

#define MINPRESSURE 10
#define MAXPRESSURE 1000

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

TouchScreen ts = TouchScreen(XP, YP, XM, YM, RX);

volatile static uint16_t framecount;
const uint8_t refreshFrames PROGMEM = 100;

uint8_t currentScreen;  // acá guardo la pantalla activa
uint8_t prevScreen;     // acá guardo la pantalla anterior

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
}

void loop(){}

{
  DEBUG_PRINT(F("v"));
  DEBUG_PRINTLN(F(VERSION));
  DEBUG_PRINT(F("TFT size is "));
  DEBUG_PRINT(tft.width());
  DEBUG_PRINT(F("x"));
  DEBUG_PRINTLN(tft.height());

  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(0);

  tft.fillScreen(BLACK);

  while (1) {
    TSPoint p = ts.getPoint();

    // if sharing pins, you'll need to fix the directions of the touchscreen
    // pins pinMode(XP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    // pinMode(YM, OUTPUT);

    // si la pantalla está siendo presionada se mappea el resultado de
    // getPoint() a un punto valido del cursor en la pantalla no es necesario
    // refrescar la pantalla si no cambió nada ni está siendo presionada
    if (p.z > MINPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = map(p.y, TS_MINY, TS_MAXY - 60, tft.height(), 0);

      if (currentScreen != 0 && (p.y < -1)) {
        HomeScreen();
      }

      switch (currentScreen) {}
    }
  }
}