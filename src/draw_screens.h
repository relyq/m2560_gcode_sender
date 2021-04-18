#ifndef DRAW_SCREENS_H
#define DRAW_SCREENS_H

#include "common_defs.h"

class Adafruit_TFTLCD;
class Adafruit_GFX_Button;

void drawHomeScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsHome);
void drawSDScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsSD);
void drawCalibrationScreen(Adafruit_TFTLCD* tft);

#endif