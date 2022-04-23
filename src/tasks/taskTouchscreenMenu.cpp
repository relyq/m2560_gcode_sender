#include "DEBUG_things.h"
#include "common_defs.h"
#include "controllers/controllers.h"
#include "gcode_tasks.h"
#include "src/Adafruit_GFX.h"     // Core graphics library
#include "src/Adafruit_TFTLCD.h"  // Hardware-specific library
#include "src/TouchScreen.h"
#include "touchscreen_config.h"
#include "views/draw_screens.h"

Screens currentScreen;  // acá guardo la pantalla activa

// si hay un programa seleccionado aparece el boton de inicio
bool fileSelected = 0;

uint8_t currentFile = 0xff;

extern char** files;

void taskTouchscreenMenu(void* pvParameters) {
  Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

  TouchScreen ts = TouchScreen(XP, YP, XM, YM, RX);

  static Adafruit_GFX_Button buttonsHome[8];
  static Adafruit_GFX_Button buttonsMove[24];
  static Adafruit_GFX_Button buttonsSD[4];
  static Adafruit_GFX_Button buttonsConfig[1];

  DEBUG_PRINTLN(F("Touchscreen task started."));

  uint16_t lcd_id = tft.readID();
  uint8_t lcd_rotation = 0;

  switch (lcd_id) {
    case 0x9325: {
      lcd_rotation = 1;
      break;
    }
    case 0x9341: {
      lcd_rotation = 3;
      break;
    }
    case 0xC0C0: {  // 0x9341
      DEBUG_PRINT(F("UNRECOGNIZED DRIVER (0x"));
      DEBUG_PRINTHEX(lcd_id);
      DEBUG_PRINTLN(F(") USING 0x9341 DRIVER"));
      lcd_id = 0x9341;
      lcd_rotation = 3;
      break;
    }
    default: {
      DEBUG_PRINT(F("UNRECOGNIZED DRIVER (0x"));
      DEBUG_PRINTHEX(lcd_id);
      DEBUG_PRINTLN(F(") USING 0x9341 DRIVER"));
      lcd_id = 0x9341;
      lcd_rotation = 3;
      break;
    }
  }

  DEBUG_PRINT(F("LCD driver reported by screen: 0x"));
  DEBUG_PRINTLNHEX(lcd_id);
  DEBUG_PRINT(F("TFT size is "));
  DEBUG_PRINT(tft.width());
  DEBUG_PRINT(F("x"));
  DEBUG_PRINTLN(tft.height());

  tft.reset();
  tft.begin(lcd_id);
  tft.setRotation(lcd_rotation);

  tft.fillScreen(BLACK);

  currentScreen = Screens::Home;
  drawHomeScreen(&tft, buttonsHome, NULL);

  while (1) {
    TSPoint p = ts.getPoint();

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    if (p.z > MINPRESSURE) {
      // el modulo tactil tiene 60 puntos no dibujables en la pantalla
      TSPoint pointTmp = p;

      p.y = map(pointTmp.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.x = map(pointTmp.y, TS_MINY, TS_MAXY - 60, 0, tft.width());

      if ((currentScreen != Screens::Home && currentScreen != Screens::SD) &&
          (p.x > 320)) {
        currentScreen = Screens::Home;
        if (!fileSelected) {
          drawHomeScreen(&tft, buttonsHome, NULL);
        } else {
          drawHomeScreen(&tft, buttonsHome, files[currentFile]);
        }
      }

      switch (currentScreen) {
        case Screens::Home: {
          homeController(&tft, buttonsHome, buttonsSD, buttonsMove,
                         buttonsConfig, p);
          break;
        }
        case Screens::SD: {
          sdController(&tft, buttonsSD, buttonsHome, p);
          break;
        }
        case Screens::Move: {
          moveController(&tft, buttonsMove, buttonsHome, p);
          break;
        }
        case Screens::Config: {
          configController(&tft, buttonsConfig, buttonsHome, p);
          break;
        }
        case Screens::Done: {
          doneController(&tft, buttonsHome);
          break;
        }
      }
    }
    vTaskDelay(15 / portTICK_PERIOD_MS);
  }
}