#include "draw_screens.h"

void drawConfigScreen(Adafruit_TFTLCD* tft,
                      Adafruit_GFX_Button* buttonsConfig) {
  DEBUG_PRINTLN("drawing config screen");
  tft->fillScreen(BLACK);

  buttonsConfig[0].initButton(tft, 160, 220, 230, 40, WHITE, BLACK, WHITE,
                              "Volver", 3);
  buttonsConfig[0].drawButton();
}