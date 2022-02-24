#define DEBUG_ENABLED 0

#ifdef DEBUG_ENABLED
#define DEBUG_BEGIN(x) Serial.begin(x)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
#define DEBUG_PRINTHEX(x) Serial.print(x, HEX)
#define DEBUG_PRINTLNDEC(x) Serial.println(x, DEC)
#define DEBUG_PRINTLNHEX(x) Serial.println(x, HEX)
#else
#define DEBUG_BEGIN(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTHEX(x)
#define DEBUG_PRINTLNDEC(x)
#define DEBUG_PRINTLNHEX(x)
#endif
