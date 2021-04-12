// SCREENS
#include "draw_screens.h"

#include "src/Adafruit_TFTLCD.h"

void drawHomeScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* homeButtons) {
  tft->fillScreen(BLACK);

  tft->setCursor(10, 65);
  tft->setTextSize(3);
  tft->setTextColor(WHITE, BLACK);
  tft->print(F("X: 0.00"));
  tft->setCursor(10, 65 + ((CHARACTER_HEIGHT + 2) * 3));
  tft->print(F("Y: 0.00"));
  tft->setCursor(10, 65 + ((CHARACTER_HEIGHT + 2) * 3 * 2));
  tft->print(F("Z: 0.00"));
}