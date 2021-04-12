#ifndef DRAW_SCREENS_H
#define DRAW_SCREENS_H

#include "common_defs.h"

class Adafruit_TFTLCD;
class Adafruit_GFX_Button;

void drawHomeScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* homeButtons);

#endif