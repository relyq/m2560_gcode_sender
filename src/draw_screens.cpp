// SCREENS
#include "draw_screens.h"

#include <math.h>

#include "Arduino_FreeRTOS.h"
#include "src/Adafruit_TFTLCD.h"

void drawHomeScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsHome,
                    const char* currentFileName) {
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
    tft->setTextSize(2);
    tft->setCursor(10, 240 - (CHARACTER_HEIGHT * 2) - 5);
    tft->print(currentFileName);

    buttonsHome[3].initButtonUL(tft, 190, 200, 130, 40, WHITE, GREEN, WHITE,
                                "Inicio", 3);
    buttonsHome[3].drawRectButton();
  }
}

void drawMoveScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsMove,
                    double stepsXY, double stepsZ) {
  tft->fillScreen(BLACK);
  tft->setTextSize(2);
  tft->setTextColor(WHITE, BLACK);

  uint8_t pepeglad = 0;

  if (pepeglad) {
    tft->drawFastHLine(5, 180, 310, WHITE);

    tft->drawFastHLine(0, 90, 320, WHITE);
    tft->drawFastVLine(220, 0, 240, WHITE);
  }

  buttonsMove[17].initButtonUL(tft, 0, 160, 53, 40, BLACK, BLACK, WHITE, "z",
                               2);
  buttonsMove[17].drawRectButton();

  buttonsMove[18].initButtonUL(tft, 0, 200, 53, 40, BLACK, BLACK, WHITE, "xy",
                               2);
  buttonsMove[18].drawRectButton();

  tft->drawFastHLine(0, 160, 53 * 5, WHITE);
  // tft->drawFastHLine(0, 200, 320, WHITE);
  tft->drawFastVLine(53, 160, 80, WHITE);
  tft->drawFastVLine((53 * 2), 160, 80, WHITE);
  tft->drawFastVLine((53 * 3), 160, 80, WHITE);
  tft->drawFastVLine((53 * 4), 160, 80, WHITE);
  // tft->drawFastVLine((53 * 5), 160, 80, WHITE);

  char str_stepsXY[16];
  char str_stepsZ[16];
  sprintf(str_stepsXY, "%.2f", stepsXY);
  sprintf(str_stepsZ, "%.2f", stepsZ);
  tft->setCursor(90, 140);
  tft->print(str_stepsXY);
  tft->setCursor(220, 140);
  tft->print(str_stepsZ);

  // z+1
  buttonsMove[7].initButtonUL(tft, 53, 160, 53, 40, WHITE, BLACK, WHITE, "+1",
                              2);
  buttonsMove[7].drawRectButton();

  // z-1
  buttonsMove[8].initButtonUL(tft, 53 * 2, 160, 53, 40, WHITE, BLACK, WHITE,
                              "-1", 2);
  buttonsMove[8].drawRectButton();

  // zx10
  buttonsMove[9].initButtonUL(tft, 53 * 3, 160, 53, 40, WHITE, BLACK, WHITE,
                              "x10", 2);
  buttonsMove[9].drawRectButton();

  // z/10
  buttonsMove[10].initButtonUL(tft, 53 * 4, 160, 53, 40, WHITE, BLACK, WHITE,
                               " 10", 2);
  buttonsMove[10].drawRectButton();
  /*
    buttonsMove[11].initButtonUL(tft, 53 * 5, 160, 56, 40, WHITE, BLACK, WHITE,
                                 str_stepsXY, 2);
    buttonsMove[11].drawRectButton();
  */
  // xy+1
  buttonsMove[12].initButtonUL(tft, 53, 200, 53, 41, WHITE, BLACK, WHITE, "+1",
                               2);
  buttonsMove[12].drawRectButton();

  // xy-1
  buttonsMove[13].initButtonUL(tft, 53 * 2, 200, 53, 41, WHITE, BLACK, WHITE,
                               "-1", 2);
  buttonsMove[13].drawRectButton();

  // xyx10
  buttonsMove[14].initButtonUL(tft, 53 * 3, 200, 53, 41, WHITE, BLACK, WHITE,
                               "x10", 2);
  buttonsMove[14].drawRectButton();

  // xy/10
  buttonsMove[15].initButtonUL(tft, 53 * 4, 200, 53, 41, WHITE, BLACK, WHITE,
                               " 10", 2);
  buttonsMove[15].drawRectButton();
  /*
    buttonsMove[16].initButtonUL(tft, 53 * 5, 200, 56, 41, WHITE, BLACK, WHITE,
                                 str_stepsZ, 2);
    buttonsMove[16].drawRectButton();
  */
  tft->drawRect((53 * 4) + 5, 179, 12, 2, WHITE);
  tft->drawRect((53 * 4) + 5, 219, 12, 2, WHITE);

  tft->drawRect((53 * 4) + 10, 174, 2, 2, WHITE);
  tft->drawRect((53 * 4) + 10, 184, 2, 2, WHITE);
  tft->drawRect((53 * 4) + 10, 214, 2, 2, WHITE);
  tft->drawRect((53 * 4) + 10, 224, 2, 2, WHITE);

  uint8_t centerX = 70;
  uint8_t centerX_z = 200;
  uint8_t centerY = 80;
  uint8_t halfside = 20;
  uint8_t offset = 10;

  /*
    tft->setCursor(centerX - 10, centerY - halfside - 35 - offset - 20);
    tft->print(F("Y+"));
    tft->setCursor(centerX - 10, centerY + halfside + 35 + offset + 10);
    tft->print(F("Y-"));
    tft->setCursor(centerX + halfside + 35 + offset + 10, centerY - 7);
    tft->print(F("X+"));
    tft->setCursor(centerX - halfside - 35 - offset - 30, centerY - 7);
    tft->print(F("X-"));
    tft->setCursor(centerX_z + 20, centerY - halfside - offset - 30);
    tft->print(F("Z+"));
    tft->setCursor(centerX_z + 20, centerY + halfside + offset + 20);
    tft->print(F("Z-"));
  */

  // las flechas ocupan 130x130px y 35x130px
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
  buttonsMove[5].initButtonUL(tft, centerX_z - halfside,
                              centerY - halfside - 35 - offset, halfside * 2,
                              35, RED, RED, WHITE, "", 2);
  tft->drawTriangle(centerX_z - halfside, centerY - halfside - offset,
                    centerX_z + halfside, centerY - halfside - offset,
                    centerX_z, centerY - halfside - 35 - offset, WHITE);
  // z-
  buttonsMove[6].initButtonUL(tft, centerX_z - halfside,
                              centerY + halfside + offset, halfside * 2, 35,
                              RED, RED, WHITE, "", 2);
  tft->drawTriangle(centerX_z - halfside, centerY + halfside + offset,
                    centerX_z + halfside, centerY + halfside + offset,
                    centerX_z, centerY + halfside + 35 + offset, WHITE);

  if (pepeglad) {
    buttonsMove[1].drawRectButton();
    buttonsMove[2].drawRectButton();
    buttonsMove[3].drawRectButton();
    buttonsMove[4].drawRectButton();
    buttonsMove[5].drawRectButton();
    buttonsMove[6].drawRectButton();
  }

  /*
  tft->setCursor(10, 240 - ((CHARACTER_HEIGHT + 2) * 2) * 3);
  tft->print(F("X: 0.00"));
  tft->setCursor(10, 240 - ((CHARACTER_HEIGHT + 2) * 2) * 2);
  tft->print(F("Y: 0.00"));
  tft->setCursor(10, 240 - ((CHARACTER_HEIGHT + 2) * 2));
  tft->print(F("Z: 0.00"));

    buttonsMove[0].initButtonUL(tft, 220, 175, 100, 60, WHITE, BLACK, WHITE,
                                "Volver", 2);
    buttonsMove[0].drawButton();
  */
}

void drawSDScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsSD,
                  uint8_t filecount, char** filenames) {
  tft->fillScreen(BLACK);

  tft->setTextSize(2);
  tft->setTextColor(WHITE, BLACK);
  tft->setCursor(0, 0);
  tft->print("Tarjeta SD");

  const uint8_t files_per_page = 6;

  const uint8_t total_pages = ceil((double)filecount / (double)files_per_page);
  uint8_t current_page = 0;
  uint8_t file_offset = files_per_page * current_page;
  uint8_t page_last_file = files_per_page + file_offset;
  // file_offset < i < page_last_file
  // filecount = 12
  // page 0:
  // 0 < i < 6
  // page 1:
  // 6 < i < 12
  // page 3:
  // 12 < i < 18

  // cursor should change depending on string length
  tft->setTextSize(2);
  tft->setCursor(280, 0);
  tft->print(current_page + 1);
  tft->print("/");
  tft->print(total_pages);

  if (total_pages > 1) {
    // multi page
    buttonsSD[2].initButton(tft, 255, 175, 40, 40, WHITE, BLACK, WHITE, "<", 3);
    buttonsSD[2].drawButton();
    buttonsSD[3].initButton(tft, 300, 175, 40, 40, WHITE, BLACK, WHITE, ">", 3);
    buttonsSD[3].drawButton();
    tft->setTextSize(2);
    for (size_t i = 0 + file_offset; i < page_last_file; i++) {
      if (filecount <= i) break;
      uint16_t line_x = i - file_offset < 4 ? 315 : 230;
      tft->drawLine(5, 56 + 28 * i, line_x, 56 + 28 * i, WHITE);
      tft->setCursor(5, 35 + 28 * i);
      tft->print(filenames[i]);
    }
  } else {
    // need file offset so index still corresponds to file array every page
    // i think i can do some modulo magic somewhere
    // absolute file number % files_per_page = file number in current_page
    tft->setTextSize(2);
    for (size_t i = 0 + file_offset; i < page_last_file; i++) {
      if (filecount <= i) break;
      tft->drawLine(5, 56 + 28 * i, 315, 56 + 28 * i, WHITE);
      tft->setCursor(5, 35 + 28 * i);
      tft->print(filenames[i]);
    }
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