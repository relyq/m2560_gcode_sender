// SCREENS
#include "draw_screens.h"

#include "src/Adafruit_TFTLCD.h"

void drawHomeScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsHome) {
  tft->fillScreen(BLACK);

  buttonsHome[0].initButton(tft, 160, 25, 230, 40, WHITE, BLACK, WHITE,
                            "Programas", 3);
  buttonsHome[0].drawButton();
  buttonsHome[1].initButton(tft, 80, 70, 130, 40, WHITE, BLACK, WHITE, "Mover",
                            3);
  buttonsHome[1].drawButton();
  buttonsHome[2].initButton(tft, 240, 70, 130, 40, WHITE, BLACK, WHITE,
                            "Config", 3);
  buttonsHome[2].drawButton();

  tft->setTextSize(3);
  tft->setTextColor(WHITE, BLACK);
  tft->setCursor(10, 240 - ((CHARACTER_HEIGHT + 2) * 3) * 3);
  tft->print(F("X: 0.00"));
  tft->setCursor(10, 240 - ((CHARACTER_HEIGHT + 2) * 3) * 2);
  tft->print(F("Y: 0.00"));
  tft->setCursor(10, 240 - ((CHARACTER_HEIGHT + 2) * 3));
  tft->print(F("Z: 0.00"));

  tft->setTextSize(2);
  tft->setCursor(10, 240 - (((CHARACTER_HEIGHT + 2) * 3) * 3) -
                         ((CHARACTER_HEIGHT + 2) * 2));
  tft->print(F("RPC.gcode (67%)"));
}

void drawSDScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsSD) {
  tft->fillScreen(BLACK);

  tft->setTextSize(2);
  tft->setTextColor(WHITE, BLACK);
  tft->setCursor(0, 0);
  tft->print("Tarjeta SD");
}

void drawCalibrationScreen(Adafruit_TFTLCD* tft) {
  tft->fillScreen(BLACK);

  struct target {
    uint16_t x;
    uint16_t y;
  };

  struct target t0;
  struct target t1;
  struct target t2;
  t0.x = 48;
  t0.y = 36;
  t1.x = 160;
  t1.y = 204;
  t2.x = 272;
  t2.y = 120;

  const uint8_t target_radius = 20;

  tft->fillCircle(t0.x, t0.y, target_radius, ORANGE);
  tft->drawCircle(t0.x, t0.y, target_radius, WHITE);
  tft->drawLine(t0.x - target_radius, t0.y - target_radius,
                t0.x + target_radius, t0.y + target_radius, WHITE);
  tft->drawLine(t0.x - target_radius, t0.y + target_radius,
                t0.x + target_radius, t0.y - target_radius, WHITE);

  tft->drawChar(t0.x + target_radius + 10, t0.y, '1', WHITE, BLACK, 2);

  tft->fillCircle(t1.x, t1.y, target_radius, ORANGE);
  tft->drawCircle(t1.x, t1.y, target_radius, WHITE);
  tft->drawLine(t1.x - target_radius, t1.y - target_radius,
                t1.x + target_radius, t1.y + target_radius, WHITE);
  tft->drawLine(t1.x - target_radius, t1.y + target_radius,
                t1.x + target_radius, t1.y - target_radius, WHITE);

  tft->drawChar(t1.x + target_radius + 10, t1.y, '2', WHITE, BLACK, 2);

  tft->fillCircle(t2.x, t2.y, target_radius, ORANGE);
  tft->drawCircle(t2.x, t2.y, target_radius, WHITE);
  tft->drawLine(t2.x - target_radius, t2.y - target_radius,
                t2.x + target_radius, t2.y + target_radius, WHITE);
  tft->drawLine(t2.x - target_radius, t2.y + target_radius,
                t2.x + target_radius, t2.y - target_radius, WHITE);

  tft->drawChar(t2.x + target_radius + 10, t2.y, '3', WHITE, BLACK, 2);
}