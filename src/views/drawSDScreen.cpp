#include "draw_screens.h"

void drawSDScreen(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsSD,
                  uint8_t filecount, char** filenames,
                  const uint8_t currentFile) {
  DEBUG_PRINTLN("drawing file explorer");
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

  if (currentFile < files_per_page) {
    tft->fillCircle(300, 42 + 28 * currentFile, 5, WHITE);
  }

  char buffer[MAX_FILENAME_SD + 1];
  buffer[MAX_FILENAME_SD] = '\0';

  if (total_pages > 1) {
    // multi page
    tft->setTextSize(2);
    for (size_t i = 0 + file_offset; i < page_last_file; i++) {
      if (filecount <= i) break;
      uint16_t line_x = i - file_offset < 4 ? 315 : 230;
      tft->drawLine(5, 56 + 28 * i, line_x, 56 + 28 * i, WHITE);
      tft->setCursor(5, 35 + 28 * i);
      strncpy(buffer, filenames[i], MAX_FILENAME_SD);
      tft->print(buffer);
    }
    buttonsSD[2].initButton(tft, 255, 175, 40, 40, WHITE, THEME_ACCENT, WHITE,
                            "<", 3);
    buttonsSD[2].drawButton();
    buttonsSD[3].initButton(tft, 300, 175, 40, 40, WHITE, THEME_ACCENT, WHITE,
                            ">", 3);
    buttonsSD[3].drawButton();
  } else {
    // need file offset so index still corresponds to file array every page
    // i think i can do some modulo magic somewhere
    // absolute file number % files_per_page = file number in current_page
    tft->setTextSize(2);
    for (size_t i = 0 + file_offset; i < page_last_file; i++) {
      if (filecount <= i) break;
      tft->drawLine(5, 56 + 28 * i, 315, 56 + 28 * i, WHITE);
      tft->setCursor(5, 35 + 28 * i);
      strncpy(buffer, filenames[i], MAX_FILENAME_SD);
      tft->print(buffer);
    }
  }

  buttonsSD[0].initButton(tft, 80, 220, 160, 40, WHITE, THEME_ACCENT, WHITE,
                          "Volver", 3);
  buttonsSD[0].drawButton();

  buttonsSD[1].initButton(tft, 240, 220, 160, 40, WHITE, THEME_PRIMARY, WHITE,
                          "Aceptar", 3);
}
