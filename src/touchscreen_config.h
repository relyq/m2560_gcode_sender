#ifndef TOUCHSCREEN_CONFIG_H
#define TOUCHSCREEN_CONFIG_H

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3  // Chip Select goes to Analog 3
#define LCD_CD A2  // Command/Data goes to Analog 2
#define LCD_WR A1  // LCD Write goes to Analog 1
#define LCD_RD A0  // LCD Read goes to Analog 0
// optional
#define LCD_RESET A4  // Can alternately just connect to Arduino's reset pin

// touchscreen
#define TS0 0
#define TS1 1
#if TS0
#define YP A3   // must be an analog pin // LCD CS
#define XM A2   // must be an analog pin // LCD RS - COMMAND/DATA
#define YM 9    // can be a digital pin  // LCD D1
#define XP 8    // can be a digital pin  // LCD D0
#define RX 350  // resistance between x+ and x-
#elif TS1
#define YP A1   // must be an analog pin // LCD WR
#define XM A2   // must be an analog pin // LCD RS - COMMAND/DATA
#define YM 7    // can be a digital pin  // LCD D1
#define XP 6    // can be a digital pin  // LCD D6
#define RX 615  // resistance between x+ and x-
#endif

// puntos maximos y minimos de la pantalla tactil, contando el espacio
// no-dibujable
#if TS0
#define TS_MINX 120
#define TS_MINY 75
#define TS_MAXX 900
#define TS_MAXY 950
#elif TS1
#define TS_MINX 150
#define TS_MINY 950
#define TS_MAXX 920
#define TS_MAXY 200
#endif

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#endif