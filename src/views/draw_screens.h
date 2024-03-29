#ifndef DRAW_SCREENS_H
#define DRAW_SCREENS_H

#include <stdint.h>

#include "DEBUG_things.h"
#include "color_theme.h"
#include "common_defs.h"
#include "src/Adafruit_TFTLCD.h"

class Adafruit_TFTLCD;
class Adafruit_GFX_Button;

void drawHomeScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsHome,
                    const char* currentFileName);
void drawMoveScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsMove,
                    double stepsXY, double stepsZ);
void drawSDScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsSD,
                  uint8_t filecount, char** filenames,
                  const uint8_t currentFile);
void drawConfigScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsConfig);

#endif