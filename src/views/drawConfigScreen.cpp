#include "draw_screens.h"

extern bool router_mode;

void drawConfigScreen(Adafruit_TFTLCD* tft,
                      Adafruit_GFX_Button* buttonsConfig) {
  DEBUG_PRINTLN("drawing config screen");
  tft->fillScreen(THEME_BACKGROUND);

  tft->setTextSize(BUTTON_TEXTSIZE);
  tft->setTextColor(WHITE, THEME_BACKGROUND);

  tft->setCursor(20, 30);
  tft->print("Laser");

  tft->setCursor(320 - (6 * CHARACTER_WIDTH * BUTTON_TEXTSIZE), 30);
  tft->print("Router");

  buttonsConfig[1].initButton(tft, 320 / 2, 40, 80, 40, WHITE, LIGHTGREY, WHITE,
                              "", 3);
  buttonsConfig[1].drawButton();

  tft->drawCircle((320 / 2) - 17, 40, 15, WHITE);
  tft->drawCircle((320 / 2) + 17, 40, 15, WHITE);

  if (router_mode) {
    tft->fillCircle((320 / 2) - 17, 40, 14, BLACK);
    tft->fillCircle((320 / 2) + 17, 40, 14, THEME_PRIMARY);
  } else {
    tft->fillCircle((320 / 2) - 17, 40, 14, THEME_PRIMARY);
    tft->fillCircle((320 / 2) + 17, 40, 14, BLACK);
  }

  buttonsConfig[0].initButton(tft, 160, 220, 230, 40, WHITE, THEME_ACCENT,
                              WHITE, "Volver", 3);
  buttonsConfig[0].drawButton();
}