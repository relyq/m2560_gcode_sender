// SCREENS
#include "draw_screens.h"

#include "Arduino_FreeRTOS.h"
#include "src/Adafruit_TFTLCD.h"

void drawHomeScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsHome,
                    const char* currentFileName) {
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

  if (currentFileName) {
    tft->setTextSize(2);
    tft->setCursor(10, 240 - (((CHARACTER_HEIGHT + 2) * 3) * 3) -
                           ((CHARACTER_HEIGHT + 2) * 2));
    tft->print(currentFileName);

    buttonsHome[3].initButtonUL(tft, 190, 200, 130, 40, WHITE, GREEN, WHITE,
                                "Inicio", 3);
    buttonsHome[3].drawRectButton();
  }
}

void drawMoveScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsMove) {
  tft->fillScreen(BLACK);
  tft->setTextSize(2);
  tft->setTextColor(WHITE, BLACK);

  uint8_t pepeglad = 0;

  if (pepeglad) {
    tft->drawFastHLine(5, 180, 310, WHITE);

    tft->drawFastHLine(0, 90, 320, WHITE);
    tft->drawFastVLine(220, 0, 240, WHITE);
  }

  tft->drawFastVLine(100, 185, 50, WHITE);
  tft->drawFastHLine(90, 240 - 21, 10, WHITE);

  tft->setCursor(105, 180 + (((240 - 20) - 180) / 2) - 7);
  tft->print(F("10mm"));
  tft->setCursor(105, 240 - ((CHARACTER_HEIGHT + 2) * 2));
  tft->print(F("10mm"));

  uint8_t centerX = 130;
  uint8_t centerY = 90;
  uint8_t halfside = 20;
  uint8_t offset = 10;

  tft->setCursor(centerX - 10, centerY - halfside - 35 - offset - 20);
  tft->print(F("Y+"));
  tft->setCursor(centerX - 10, centerY + halfside + 35 + offset + 10);
  tft->print(F("Y-"));
  tft->setCursor(centerX + halfside + 35 + offset + 10, centerY - 7);
  tft->print(F("X+"));
  tft->setCursor(centerX - halfside - 35 - offset - 30, centerY - 7);
  tft->print(F("X-"));
  tft->setCursor(270 + 20, centerY - halfside - offset - 30);
  tft->print(F("Z+"));
  tft->setCursor(270 + 20, centerY + halfside + offset + 20);
  tft->print(F("Z-"));

  // y+
  buttonsMove[1].initButtonUL(tft, centerX - halfside,
                              centerY - halfside - 35 - offset, halfside * 2,
                              35, RED, RED, WHITE, "", 2);
  tft->drawTriangle(centerX - halfside, centerY - halfside - offset,
                    centerX + halfside, centerY - halfside - offset, centerX,
                    centerY - halfside - 35 - offset, WHITE);
  // y-
  buttonsMove[2].initButtonUL(tft, centerX - halfside,
                              centerY + halfside + offset, halfside * 2, 35,
                              RED, RED, WHITE, "", 2);
  tft->drawTriangle(centerX - halfside, centerY + halfside + offset,
                    centerX + halfside, centerY + halfside + offset, centerX,
                    centerY + halfside + 35 + offset, WHITE);
  // x+
  buttonsMove[3].initButtonUL(tft, centerX + halfside + offset,
                              centerY - halfside, 35, halfside * 2, RED, RED,
                              WHITE, "", 2);
  tft->drawTriangle(centerX + halfside + offset, centerY - halfside,
                    centerX + halfside + offset, centerY + halfside,
                    centerX + halfside + 35 + offset, centerY, WHITE);
  // x-
  buttonsMove[4].initButtonUL(tft, centerX - halfside - 35 - offset,
                              centerY - halfside, 35, halfside * 2, RED, RED,
                              WHITE, "", 2);
  tft->drawTriangle(centerX - halfside - offset, centerY - halfside,
                    centerX - halfside - offset, centerY + halfside,
                    centerX - halfside - 35 - offset, centerY, WHITE);
  // z+
  buttonsMove[5].initButtonUL(tft, 270 - halfside,
                              centerY - halfside - 35 - offset, halfside * 2,
                              35, RED, RED, WHITE, "", 2);
  tft->drawTriangle(270 - halfside, centerY - halfside - offset, 270 + halfside,
                    centerY - halfside - offset, 270,
                    centerY - halfside - 35 - offset, WHITE);
  // z-
  buttonsMove[6].initButtonUL(tft, 270 - halfside, centerY + halfside + offset,
                              halfside * 2, 35, RED, RED, WHITE, "", 2);
  tft->drawTriangle(270 - halfside, centerY + halfside + offset, 270 + halfside,
                    centerY + halfside + offset, 270,
                    centerY + halfside + 35 + offset, WHITE);

  if (pepeglad) {
    buttonsMove[1].drawRectButton();
    buttonsMove[2].drawRectButton();
    buttonsMove[3].drawRectButton();
    buttonsMove[4].drawRectButton();
    buttonsMove[5].drawRectButton();
    buttonsMove[6].drawRectButton();
  }

  tft->setCursor(10, 240 - ((CHARACTER_HEIGHT + 2) * 2) * 3);
  tft->print(F("X: 0.00"));
  tft->setCursor(10, 240 - ((CHARACTER_HEIGHT + 2) * 2) * 2);
  tft->print(F("Y: 0.00"));
  tft->setCursor(10, 240 - ((CHARACTER_HEIGHT + 2) * 2));
  tft->print(F("Z: 0.00"));

  // 150, 210 // 5, 150 // 5, 5
  buttonsMove[7].initButtonUL(tft, 5, 140, 60, 30, WHITE, BLACK, WHITE, "Cero",
                              2);
  buttonsMove[7].drawButton();

  buttonsMove[8].initButtonUL(tft, 220, 175, 100, 30, WHITE, BLACK, WHITE,
                              "Probe", 2);
  buttonsMove[8].drawButton();

  buttonsMove[0].initButtonUL(tft, 220, 210, 100, 30, WHITE, BLACK, WHITE,
                              "Aceptar", 2);
  buttonsMove[0].drawButton();
}

void drawSDScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsSD,
                  uint8_t filecount, char (*filenames)[20]) {
  tft->fillScreen(BLACK);

  tft->setTextSize(2);
  tft->setTextColor(WHITE, BLACK);
  tft->setCursor(0, 0);
  tft->print("Tarjeta SD");

  for (uint8_t i = 0; i < filecount; i++) {
    tft->drawLine(5, 56 + 28 * i, 315, 56 + 28 * i, WHITE);
    tft->setCursor(5, 35 + 28 * i);
    tft->print(filenames[i]);
  }

  buttonsSD[0].initButton(tft, 80, 220, 160, 40, WHITE, BLACK, WHITE, "Volver",
                          3);
  buttonsSD[0].drawButton();

  buttonsSD[1].initButton(tft, 240, 220, 160, 40, WHITE, BLACK, WHITE,
                          "Aceptar", 3);
}

void drawConfigScreen(Adafruit_TFTLCD* tft,
                      Adafruit_GFX_Button* buttonsConfig) {
  tft->fillScreen(BLACK);

  buttonsConfig[0].initButton(tft, 160, 220, 230, 40, WHITE, BLACK, WHITE,
                              "Volver", 3);
  buttonsConfig[0].drawButton();
}