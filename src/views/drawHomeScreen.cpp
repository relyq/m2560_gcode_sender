#include "draw_screens.h"

void drawHomeScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsHome,
                    const char* currentFileName) {
  DEBUG_PRINTLN("drawing home screen");
  tft->fillScreen(BLACK);

  buttonsHome[0].initButton(tft, 160, 25, 230, 40, WHITE, BLACK, WHITE,
                            "Programas", 3);
  buttonsHome[0].drawButton();

  buttonsHome[4].initButton(tft, 80, 160, 130, 40, WHITE, BLACK, WHITE, "Home",
                            3);
  buttonsHome[4].drawButton();

  buttonsHome[1].initButton(tft, 80, 70, 130, 40, WHITE, BLACK, WHITE, "Mover",
                            3);
  buttonsHome[1].drawButton();

  buttonsHome[2].initButton(tft, 240, 70, 130, 40, WHITE, BLACK, WHITE,
                            "Config", 3);
  buttonsHome[2].drawButton();

  buttonsHome[5].initButton(tft, 240, 160, 130, 40, WHITE, BLACK, WHITE, "Ir 0",
                            3);
  buttonsHome[5].drawButton();

  buttonsHome[6].initButton(tft, 80, 115, 130, 40, WHITE, BLACK, WHITE, "Probe",
                            3);
  buttonsHome[6].drawButton();

  buttonsHome[7].initButton(tft, 240, 115, 130, 40, WHITE, BLACK, WHITE,
                            "Set 0", 3);
  buttonsHome[7].drawButton();

  if (currentFileName) {
    char buffer[MAX_FILENAME_HOME + 1];
    buffer[MAX_FILENAME_HOME] = '\0';
    strncpy(buffer, currentFileName, MAX_FILENAME_HOME);

    tft->setTextSize(2);
    tft->setCursor(10, 240 - (CHARACTER_HEIGHT * 2) - 5);
    tft->print(buffer);

    buttonsHome[3].initButtonUL(tft, 190, 200, 130, 40, WHITE, GREEN, WHITE,
                                "Inicio", 3);
    buttonsHome[3].drawRectButton();
  }
}