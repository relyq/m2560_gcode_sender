#include <Arduino_FreeRTOS.h>
#include <EEPROM.h>

#include "controllers.h"
#include "eeprom_things.h"

#define BUTTON_BACK 0
#define BUTTON_LASERROUTER 1
#define BUTTON_COUNT (BUTTON_LASERROUTER + 1)

extern Screens currentScreen;
extern bool fileSelected;
extern uint8_t currentFile;
extern char** files;

extern bool router_mode;

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

    case BUTTON_LASERROUTER: {
      router_mode = !router_mode;
      EEPROM.update(EEPROMDIR_ROUTERMODE, router_mode);

      if (router_mode) {
        tft->fillCircle((320 / 2) - 17, 40, 14, BLACK);
        tft->fillCircle((320 / 2) + 17, 40, 14, THEME_PRIMARY);
      } else {
        tft->fillCircle((320 / 2) - 17, 40, 14, THEME_PRIMARY);
        tft->fillCircle((320 / 2) + 17, 40, 14, BLACK);
      }
      vTaskDelay(pdMS_TO_TICKS(150));
    }

    default:
      break;
  }
}