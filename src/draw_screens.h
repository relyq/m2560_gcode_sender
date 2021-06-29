#ifndef DRAW_SCREENS_H
#define DRAW_SCREENS_H

#include <stdint.h>

#include "common_defs.h"

class Adafruit_TFTLCD;
class Adafruit_GFX_Button;

void drawHomeScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsHome,
                    const char* currentFileName);
void drawMoveScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsMove);
void drawSDScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsSD,
                  uint8_t filecount, char (*filenames)[20]);
void drawConfigScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsConfig);

#endif