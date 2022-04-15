#include "controllers.h"

extern Screens currentScreen;
extern bool fileSelected;
extern uint8_t currentFile;
extern char** files;

void configController(Adafruit_TFTLCD* tft, Adafruit_GFX_Button* buttonsConfig,
                      Adafruit_GFX_Button* buttonsHome, const TSPoint p) {
  if (buttonsConfig[0].contains(p.x, p.y)) {
    currentScreen = Screens::Home;
    if (!fileSelected) {
      drawHomeScreen(tft, buttonsHome, NULL);
    } else {
      drawHomeScreen(tft, buttonsHome, files[currentFile]);
    }
  }
}