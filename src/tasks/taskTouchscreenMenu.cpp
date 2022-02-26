#include "DEBUG_things.h"
#include "common_defs.h"
#include "draw_screens.h"
#include "gcode_tasks.h"
#include "src/Adafruit_GFX.h"     // Core graphics library
#include "src/Adafruit_TFTLCD.h"  // Hardware-specific library
#include "src/TouchScreen.h"
#include "touchscreen_config.h"

extern QueueHandle_t qGcodeLine;
extern QueueHandle_t qGcodeFile;

void taskTouchscreenMenu(void* pvParameters) {
  Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

  TouchScreen ts = TouchScreen(XP, YP, XM, YM, RX);

  static Adafruit_GFX_Button buttonsHome[8];
  static Adafruit_GFX_Button buttonsMove[24];
  static Adafruit_GFX_Button buttonsSD[4];
  static Adafruit_GFX_Button buttonsConfig[1];

  Screens currentScreen;  // acá guardo la pantalla activa
  // Screens prevScreen;     // acá guardo la pantalla anterior

  static uint16_t feedrate = 200;
  static double stepsXY = 10;
  static double step_multiplierXY = 10;
  static double stepsZ = 1;
  static double step_multiplierZ = 1;
  static const uint8_t max_multiplierZ = 10;
  static const uint8_t max_multiplierXY = 100;
  static const uint8_t max_stepsZ = 50;
  static const uint16_t max_stepsXY = 300;
  static const double min_steps = 0.01;

  static uint8_t currentFile = 0xff;
  static uint8_t selectedFile = 0xff;
  extern char** files;
  extern uint8_t filecount;

  static uint8_t current_page = 0;
  const uint8_t files_per_page = 6;

  // si hay un programa seleccionado aparece el boton de inicio
  bool fileSelected = 0;

  DEBUG_PRINTLN(F("Touchscreen task started."));

  uint16_t lcd_id = tft.readID();
  uint8_t lcd_rotation = 0;

  switch (lcd_id) {
    case 0x9325: {
      lcd_rotation = 1;
      break;
    }
    case 0x9341: {
      lcd_rotation = 3;
      break;
    }
    case 0xC0C0: {  // 0x9341
      DEBUG_PRINT(F("UNRECOGNIZED DRIVER (0x"));
      DEBUG_PRINTHEX(lcd_id);
      DEBUG_PRINTLN(F(") USING 0x9341 DRIVER"));
      lcd_id = 0x9341;
      lcd_rotation = 3;
      break;
    }
    default: {
      DEBUG_PRINT(F("UNRECOGNIZED DRIVER (0x"));
      DEBUG_PRINTHEX(lcd_id);
      DEBUG_PRINTLN(F(") USING 0x9341 DRIVER"));
      lcd_id = 0x9341;
      lcd_rotation = 3;
      break;
    }
  }

  DEBUG_PRINT(F("LCD driver reported by screen: 0x"));
  DEBUG_PRINTLNHEX(lcd_id);
  DEBUG_PRINT(F("TFT size is "));
  DEBUG_PRINT(tft.width());
  DEBUG_PRINT(F("x"));
  DEBUG_PRINTLN(tft.height());

  tft.reset();
  tft.begin(lcd_id);
  tft.setRotation(lcd_rotation);

  tft.fillScreen(BLACK);

  currentScreen = Screens::Home;
  drawHomeScreen(&tft, buttonsHome, NULL);

  while (1) {
    TSPoint p = ts.getPoint();

    if (p.z > MINPRESSURE) {
      // el modulo tactil tiene 60 puntos no dibujables en la pantalla
      TSPoint pointTmp = p;

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      DEBUG_PRINT(F("x: "));
      DEBUG_PRINT(p.x);
      DEBUG_PRINT(F("\ty: "));
      DEBUG_PRINTLN(p.y);

      p.y = map(pointTmp.x, TS_MINX, TS_MAXX, tft.height(), 0);
      p.x = map(pointTmp.y, TS_MINY, TS_MAXY - 60, 0, tft.width());

      if (currentScreen != Screens::Home && (p.x > 320)) {
        currentScreen = Screens::Home;
        if (!fileSelected) {
          drawHomeScreen(&tft, buttonsHome, NULL);
        } else {
          drawHomeScreen(&tft, buttonsHome, files[currentFile]);
        }
      }

      switch (currentScreen) {
        case Screens::Home: {
          if (buttonsHome[0].contains(p.x, p.y)) {
            currentScreen = Screens::SD;
            current_page = 0;
            drawSDScreen(&tft, buttonsSD, filecount, files);
            if (currentFile < files_per_page) {
              tft.fillCircle(300, 42 + 28 * currentFile, 5, WHITE);
            }
          } else if (buttonsHome[1].contains(p.x, p.y)) {
            currentScreen = Screens::Move;
            drawMoveScreen(&tft, buttonsMove, stepsXY, stepsZ);
          } else if (buttonsHome[2].contains(p.x, p.y)) {
            currentScreen = Screens::Config;
            drawConfigScreen(&tft, buttonsConfig);
          } else if (buttonsHome[4].contains(p.x, p.y)) {
            // homing
            xQueueSend(qGcodeLine, "$H\n", portMAX_DELAY);
            vTaskDelay(500 / portTICK_PERIOD_MS);
          } else if (buttonsHome[5].contains(p.x, p.y)) {
            // go to zero
            // xQueueSend(qGcodeLine, "G28G91X0Y0Z0\n", portMAX_DELAY); // esto
            // deberia funcionar pero esto es lo que hace bCNC
            xQueueSend(qGcodeLine, "G90\n", portMAX_DELAY);
            xQueueSend(qGcodeLine, "G0Z3\n", portMAX_DELAY);  // safe z
            xQueueSend(qGcodeLine, "G0X0Y0\n", portMAX_DELAY);
            xQueueSend(qGcodeLine, "G0Z0\n", portMAX_DELAY);
            vTaskDelay(500 / portTICK_PERIOD_MS);
          } else if (buttonsHome[6].contains(p.x, p.y)) {
            // probe
            xQueueSend(qGcodeLine, "G38.3Z-25F30.0\n", portMAX_DELAY);
            vTaskDelay(500 / portTICK_PERIOD_MS);
          } else if (buttonsHome[7].contains(p.x, p.y)) {
            // set zero
            xQueueSend(qGcodeLine, "G10L20P1X0Y0Z0\n", portMAX_DELAY);
            vTaskDelay(500 / portTICK_PERIOD_MS);
          } else if (buttonsHome[3].contains(p.x, p.y)) {
            // send file
            drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            xQueueSend(qGcodeFile, files[currentFile], portMAX_DELAY);
            xQueueSend(qGcodeLine, "G28\n", portMAX_DELAY);

            tft.fillScreen(BLACK);
            tft.setTextSize(3);
            tft.setCursor((320 - (CHARACTER_WIDTH * 3 * 17)) / 2,
                          120 - (CHARACTER_HEIGHT * 3));
            tft.print("Trabajo terminado");
            currentScreen = Screens::Done;

            vTaskDelay(15 / portTICK_PERIOD_MS);
          }
          break;
        }
        case Screens::SD: {
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

          const uint8_t total_pages =
              ceil((double)filecount / (double)files_per_page);

          if (buttonsSD[0].contains(p.x, p.y)) {
            confirmButton = 0;
            selectedFile = currentFile;

            currentScreen = Screens::Home;
            if (!fileSelected) {
              drawHomeScreen(&tft, buttonsHome, NULL);
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            }
          } else if (confirmButton && buttonsSD[1].contains(p.x, p.y)) {
            confirmButton = 0;
            currentFile = selectedFile;
            fileSelected = 1;

            currentScreen = Screens::Home;

            drawHomeScreen(&tft, buttonsHome, files[currentFile]);
          } else if (buttonsSD[2].contains(p.x, p.y) && current_page > 0) {
            current_page--;
            tft.setTextSize(2);
            tft.setCursor(280, 0);
            tft.setTextColor(WHITE, BLACK);
            tft.print(current_page + 1);

            tft.fillRect(5, 35, 320, 120, BLACK);
            tft.fillRect(5, 92, 230, 108, BLACK);

            uint8_t file_offset = files_per_page * current_page;
            uint8_t page_last_file = files_per_page + file_offset;

            for (size_t i = 0 + file_offset; i < page_last_file; i++) {
              if (filecount <= i) break;
              uint8_t relative_file = i - file_offset;
              uint16_t line_x = relative_file < 4 ? 315 : 230;
              tft.drawLine(5, 56 + 28 * relative_file, line_x,
                           56 + 28 * relative_file, WHITE);
              tft.setCursor(5, 35 + 28 * relative_file);
              tft.print(files[i]);
            }

            if (currentFile > file_offset && currentFile < page_last_file) {
              tft.fillCircle(line_circle_x[currentFile % files_per_page],
                             line_circle_y[currentFile % files_per_page],
                             circle_r, WHITE);
            }
            if (selectedFile > file_offset && selectedFile < page_last_file) {
              tft.fillCircle(line_circle_x[selectedFile % files_per_page],
                             line_circle_y[selectedFile % files_per_page],
                             circle_r, WHITE);
            }
          } else if (buttonsSD[3].contains(p.x, p.y) &&
                     current_page < total_pages - 1) {
            current_page++;
            tft.setTextSize(2);
            tft.setCursor(280, 0);
            tft.setTextColor(WHITE, BLACK);
            tft.print(current_page + 1);

            tft.fillRect(5, 35, 320, 120, BLACK);
            tft.fillRect(5, 92, 230, 108, BLACK);

            uint8_t file_offset = files_per_page * current_page;
            uint8_t page_last_file = files_per_page + file_offset;

            for (size_t i = 0 + file_offset; i < page_last_file; i++) {
              if (filecount <= i) break;
              uint8_t relative_file = i - file_offset;
              uint16_t line_x = relative_file < 4 ? 315 : 230;
              tft.drawLine(5, 56 + 28 * relative_file, line_x,
                           56 + 28 * relative_file, WHITE);
              tft.setCursor(5, 35 + 28 * relative_file);
              tft.print(files[i]);
            }

            if (currentFile > file_offset && currentFile < page_last_file) {
              tft.fillCircle(line_circle_x[currentFile % files_per_page],
                             line_circle_y[currentFile % files_per_page],
                             circle_r, WHITE);
            }
            if (selectedFile > file_offset && selectedFile < page_last_file) {
              tft.fillCircle(line_circle_x[selectedFile % files_per_page],
                             line_circle_y[selectedFile % files_per_page],
                             circle_r, WHITE);
            }

          } else {
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
                  tft.fillCircle(
                      line_circle_x[j], line_circle_y[j], circle_r,
                      j + file_offset == selectedFile ? WHITE : BLACK);
                }
                if (!confirmButton) {
                  buttonsSD[1].drawButton();
                  confirmButton = 1;
                }
                break;
              }
            }
          }
          break;
        }
        case Screens::Move: {
          char str_rapid[10] = "G91 G1 F";
          char str_tmp[64];
          char command[128];
          char str_stepsXY[16];
          char str_stepsZ[16];

          strcpy(command, str_rapid);
          sprintf(str_tmp, "%d ", feedrate);
          strcat(command, str_tmp);

          tft.setTextColor(WHITE, BLACK);

          if (buttonsMove[0].contains(p.x, p.y)) {
            currentScreen = Screens::Home;
            if (!fileSelected) {
              drawHomeScreen(&tft, buttonsHome, NULL);
              break;
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
              break;
            }
          } else if (buttonsMove[1].contains(p.x, p.y)) {
            // y+
            sprintf(str_tmp, "Y%f\n", stepsXY);
            strcat(command, str_tmp);
            xQueueSend(qGcodeLine, command, portMAX_DELAY);
          } else if (buttonsMove[2].contains(p.x, p.y)) {
            // y-
            sprintf(str_tmp, "Y-%f\n", stepsXY);
            strcat(command, str_tmp);
            xQueueSend(qGcodeLine, command, portMAX_DELAY);
          } else if (buttonsMove[3].contains(p.x, p.y)) {
            // x+
            sprintf(str_tmp, "X%f\n", stepsXY);
            strcat(command, str_tmp);
            xQueueSend(qGcodeLine, command, portMAX_DELAY);
          } else if (buttonsMove[4].contains(p.x, p.y)) {
            // x-
            sprintf(str_tmp, "X-%f\n", stepsXY);
            strcat(command, str_tmp);
            xQueueSend(qGcodeLine, command, portMAX_DELAY);
          } else if (buttonsMove[5].contains(p.x, p.y)) {
            // z+
            sprintf(str_tmp, "Z%f\n", stepsZ);
            strcat(command, str_tmp);
            xQueueSend(qGcodeLine, command, portMAX_DELAY);
          } else if (buttonsMove[6].contains(p.x, p.y)) {
            // z-
            sprintf(str_tmp, "Z-%f\n", stepsZ);
            strcat(command, str_tmp);
            xQueueSend(qGcodeLine, command, portMAX_DELAY);
          } else if (buttonsMove[7].contains(p.x, p.y)) {
            // z+1
            stepsZ = stepsZ + step_multiplierZ;
            if (stepsZ > step_multiplierZ * 9) {
              step_multiplierZ = step_multiplierZ * 10;
            }
            if (stepsZ > max_stepsZ) {
              stepsZ = max_stepsZ;
            }

          } else if (buttonsMove[8].contains(p.x, p.y)) {
            // z-1
            if (stepsZ > min_steps) {
              if (stepsZ <= step_multiplierZ * 1.5) {
                step_multiplierZ = step_multiplierZ / 10;
              }
              stepsZ = stepsZ - step_multiplierZ;
            }

          } else if (buttonsMove[9].contains(p.x, p.y)) {
            // zx10
            if (stepsZ * 10 >= max_stepsZ) {
              stepsZ = max_stepsZ;
              step_multiplierZ = max_multiplierZ;
            } else {
              stepsZ = stepsZ * 10;
              step_multiplierZ = step_multiplierZ * 10;
            }

          } else if (buttonsMove[10].contains(p.x, p.y)) {
            // z/10
            if (stepsZ / 10 > min_steps) {
              stepsZ = stepsZ / 10;
              step_multiplierZ = step_multiplierZ / 10;
            } else {
              stepsZ = min_steps;
              step_multiplierZ = min_steps;
            }

          } else if (buttonsMove[12].contains(p.x, p.y)) {
            // xy+1
            stepsXY = stepsXY + step_multiplierXY;
            if (stepsXY > step_multiplierXY * 9) {
              step_multiplierXY = step_multiplierXY * 10;
            }
            if (stepsXY > max_stepsXY) {
              stepsXY = max_stepsXY;
            }

          } else if (buttonsMove[13].contains(p.x, p.y)) {
            // xy-1
            if (stepsXY > min_steps) {
              if (stepsXY <= step_multiplierXY * 1.5) {
                step_multiplierXY = step_multiplierXY / 10;
              }
              stepsXY = stepsXY - step_multiplierXY;
            }

          } else if (buttonsMove[14].contains(p.x, p.y)) {
            // xyx10
            if (stepsXY * 10 >= max_stepsXY) {
              stepsXY = max_stepsXY;
              step_multiplierXY = max_multiplierXY;
            } else {
              stepsXY = stepsXY * 10;
              step_multiplierXY = step_multiplierXY * 10;
            }

          } else if (buttonsMove[15].contains(p.x, p.y)) {
            // xy/10
            if (stepsXY / 10 > min_steps) {
              stepsXY = stepsXY / 10;
              step_multiplierXY = step_multiplierXY / 10;
            } else {
              stepsXY = min_steps;
              step_multiplierXY = min_steps;
            }
          }
          sprintf(str_stepsXY, "%.2f ", stepsXY);
          sprintf(str_stepsZ, "%.2f ", stepsZ);
          tft.setCursor(90, 140);
          tft.print(str_stepsXY);
          tft.setCursor(220, 140);
          tft.print(str_stepsZ);

          DEBUG_PRINT("stepxy: ");
          DEBUG_PRINTLN(stepsXY);
          DEBUG_PRINT("mult_xy: ");
          DEBUG_PRINTLN(step_multiplierXY);
          DEBUG_PRINT("stepz: ");
          DEBUG_PRINTLN(stepsZ);
          DEBUG_PRINT("mult_z: ");
          DEBUG_PRINTLN(step_multiplierZ);

          vTaskDelay(pdMS_TO_TICKS(100));
          break;
        }
        case Screens::Config: {
          if (buttonsConfig[0].contains(p.x, p.y)) {
            currentScreen = Screens::Home;
            if (!fileSelected) {
              drawHomeScreen(&tft, buttonsHome, NULL);
            } else {
              drawHomeScreen(&tft, buttonsHome, files[currentFile]);
            }
          }
          break;
        }
        case Screens::Done: {
          xQueueSend(qGcodeLine, "G53G0X-184Y-249Z-1\n", portMAX_DELAY);
          currentScreen = Screens::Home;
          drawHomeScreen(&tft, buttonsHome, files[currentFile]);
          break;
        }
      }
    }
    vTaskDelay(15 / portTICK_PERIOD_MS);
  }
}