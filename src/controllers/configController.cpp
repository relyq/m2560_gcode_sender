#include "controllers.h"

#define BUTTON_BACK 0
#define BUTTON_COUNT (BUTTON_BACK + 1)

extern Screens currentScreen;
extern bool fileSelected;
extern uint8_t currentFile;
extern char** files;

void configController(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsConfig,
                      Adafruit_GFX_Button* buttonsHome, const TSPoint p) {
  uint8_t pressed_button = 0xff;

  for (size_t i_btn = 0; i_btn < BUTTON_COUNT; i_btn++) {
    if (buttonsConfig[i_btn].contains(p.x, p.y)) {
      pressed_button = i_btn;
      break;
    }
  }

  switch (pressed_button) {
    case BUTTON_BACK: {
      currentScreen = Screens::Home;
      if (!fileSelected) {
        drawHomeScreen(tft, buttonsHome, NULL);
      } else {
        drawHomeScreen(tft, buttonsHome, files[currentFile]);
      }
      break;
    }

    default:
      break;
  }
}