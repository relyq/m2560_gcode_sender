#ifndef __CONTROLLERS_H__
#define __CONTROLLERS_H__

#include "DEBUG_things.h"
#include "common_defs.h"
#include "src/Adafruit_TFTLCD.h"
#include "src/TouchScreen.h"
#include "views/draw_screens.h"

void homeController(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsHome,
                    Adafruit_GFX_Button* buttonsSD,
                    Adafruit_GFX_Button* buttonsMove,
                    Adafruit_GFX_Button* buttonsConfig, const TSPoint p);
void sdController(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsSD,
                  Adafruit_GFX_Button* buttonsHome, const TSPoint p);
void moveController(Adafruit_GFX* tft, Adafruit_GFX_Button* buttonsMove,
                    Adafruit_GFX_Button* buttonsHome, const TSPoint p);
void configController(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsConfig,
                      Adafruit_GFX_Button* buttonsHome, const TSPoint p);
void doneController(Adafruit_GFX* tft, Adafruit_GFX_Button* buttonsHome);

#endif  // __CONTROLLERS_H__