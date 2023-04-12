#include "controllers.h"

#define BUTTON_BACK 0
#define BUTTON_CONFIRM 1
#define BUTTON_PREVPAGE 2
#define BUTTON_NEXTPAGE 3
#define BUTTON_COUNT (BUTTON_NEXTPAGE + 1)

const uint8_t files_per_page = 6;
extern uint8_t filecount;
extern Screens currentScreen;
static uint8_t selectedFile = 0xff;
extern uint8_t currentFile;
extern bool fileSelected;
extern char** files;

void sdController(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsSD,
                  Adafruit_GFX_Button* buttonsHome, const TSPoint p) {
  uint8_t pressed_button = 0xff;

  for (size_t i_btn = 0; i_btn < BUTTON_COUNT; i_btn++) {
    if (buttonsSD[i_btn].contains(p.x, p.y)) {
      pressed_button = i_btn;
      break;
    }
  }

  static uint8_t current_page = 0;
  static bool confirmButton = 0;
  const uint16_t line_end_x[6] = {315, 315, 315, 315, 225, 225};
  const uint8_t line_start_y[6] = {28, 56, 84, 112, 140, 168};
  const uint8_t line_end_y[6] = {56, 84, 112, 140, 168, 196};
  const uint8_t line_circle_y[6] = {
      ((line_end_y[0] - line_start_y[0]) / 2) + line_start_y[0],
      ((line_end_y[1] - line_start_y[1]) / 2) + line_start_y[1],
      ((line_end_y[2] - line_start_y[2]) / 2) + line_start_y[2],
      ((line_end_y[3] - line_start_y[3]) / 2) + line_start_y[3],
      ((line_end_y[4] - line_start_y[4]) / 2) + line_start_y[4],
      ((line_end_y[5] - line_start_y[5]) / 2) + line_start_y[5],
  };
  const uint16_t line_circle_x[6] = {300, 300, 300, 300, 215, 215};
  const uint8_t circle_r = 5;

  const uint8_t total_pages = ceil((double)filecount / (double)files_per_page);

  switch (pressed_button) {
    case BUTTON_BACK: {
      confirmButton = 0;
      current_page = 0;
      selectedFile = currentFile;

      currentScreen = Screens::Home;
      if (!fileSelected) {
        drawHomeScreen(tft, buttonsHome, NULL);
      } else {
        drawHomeScreen(tft, buttonsHome, files[currentFile]);
      }
      break;
    }
    case BUTTON_CONFIRM: {
      if (confirmButton) {
        confirmButton = 0;
        current_page = 0;
        currentFile = selectedFile;
        fileSelected = 1;

        currentScreen = Screens::Home;

        drawHomeScreen(tft, buttonsHome, files[currentFile]);
      }
      break;
    }
    case BUTTON_PREVPAGE: {
      if (current_page > 0) {
        current_page--;
        tft->setTextSize(2);
        tft->setCursor(280, 0);
        tft->setTextColor(WHITE, THEME_BACKGROUND);
        tft->print(current_page + 1);

        tft->fillRect(5, 35, 320, 120, THEME_BACKGROUND);
        tft->fillRect(5, 92, 230, 108, THEME_BACKGROUND);

        uint8_t file_offset = files_per_page * current_page;
        uint8_t page_last_file = files_per_page + file_offset;

        for (size_t i = 0 + file_offset; i < page_last_file; i++) {
          if (filecount <= i) break;
          uint8_t relative_file = i - file_offset;
          uint16_t line_x = relative_file < 4 ? 315 : 230;
          tft->drawLine(5, 56 + 28 * relative_file, line_x,
                        56 + 28 * relative_file, WHITE);
          tft->setCursor(5, 35 + 28 * relative_file);
          tft->print(files[i]);
        }

        if (currentFile > file_offset && currentFile < page_last_file) {
          tft->fillCircle(line_circle_x[currentFile % files_per_page],
                          line_circle_y[currentFile % files_per_page], circle_r,
                          WHITE);
        }
        if (selectedFile > file_offset && selectedFile < page_last_file) {
          tft->fillCircle(line_circle_x[selectedFile % files_per_page],
                          line_circle_y[selectedFile % files_per_page],
                          circle_r, WHITE);
        }
      }
      break;
    }
    case BUTTON_NEXTPAGE: {
      if (current_page < total_pages - 1) {
        current_page++;
        tft->setTextSize(2);
        tft->setCursor(280, 0);
        tft->setTextColor(WHITE, THEME_BACKGROUND);
        tft->print(current_page + 1);

        tft->fillRect(5, 35, 320, 120, THEME_BACKGROUND);
        tft->fillRect(5, 92, 230, 108, THEME_BACKGROUND);

        uint8_t file_offset = files_per_page * current_page;
        uint8_t page_last_file = files_per_page + file_offset;

        for (size_t i = 0 + file_offset; i < page_last_file; i++) {
          if (filecount <= i) break;
          uint8_t relative_file = i - file_offset;
          uint16_t line_x = relative_file < 4 ? 315 : 230;
          tft->drawLine(5, 56 + 28 * relative_file, line_x,
                        56 + 28 * relative_file, WHITE);
          tft->setCursor(5, 35 + 28 * relative_file);
          tft->print(files[i]);
        }

        if (currentFile > file_offset && currentFile < page_last_file) {
          tft->fillCircle(line_circle_x[currentFile % files_per_page],
                          line_circle_y[currentFile % files_per_page], circle_r,
                          WHITE);
        }
        if (selectedFile > file_offset && selectedFile < page_last_file) {
          tft->fillCircle(line_circle_x[selectedFile % files_per_page],
                          line_circle_y[selectedFile % files_per_page],
                          circle_r, WHITE);
        }
      }
      break;
    }

    default: {
      uint8_t file_offset = files_per_page * current_page;
      uint8_t page_last_file = files_per_page + file_offset;
      for (size_t i = 0 + file_offset; i < page_last_file; i++) {
        uint8_t relative_file = i - file_offset;
        if (filecount > i &&
            (p.x >= 5 && p.x <= line_end_x[relative_file] &&
             p.y >= line_start_y[relative_file] &&
             p.y <= line_end_y[relative_file]) &&
            (selectedFile != i)) {
          selectedFile = i;
          for (size_t j = 0; j < files_per_page; j++) {
            tft->fillCircle(
                line_circle_x[j], line_circle_y[j], circle_r,
                j + file_offset == selectedFile ? WHITE : THEME_BACKGROUND);
          }
          if (!confirmButton) {
            buttonsSD[1].drawButton();
            confirmButton = 1;
          }
          break;
        }
      }
      break;
    }
  }
}