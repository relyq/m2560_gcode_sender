#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

// Color Definitions
#define BLACK 0x0000
#define LIGHTGREY 0xC618
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define NAVY 0x000F
#define DARKGREEN 0x03E0
#define DARKCYAN 0x03EF
#define MAROON 0x7800
#define PURPLE 0x780F
#define OLIVE 0x7BE0
#define DARKGREY 0x7BEF
#define ORANGE 0xFD20
#define GREENYELLOW 0xAFE5
#define PINKK 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

#define CHARACTER_WIDTH 6   // including space 1px
#define CHARACTER_HEIGHT 7  // not including space 1px
#define BUTTON_TEXTSIZE 3
#define TITLE_TEXTSIZE 2

enum class Screens { Home, SD, Move, Config };

#endif