#ifndef SD_THINGS_H
#define SD_THINGS_H

#include <SD.h>
#include <stdint.h>

int8_t SD_filecount(File dir, uint8_t* filecount, char* filenames[]);
int8_t SD_getFileName(File dir, uint8_t fnum, char* buffer);
int8_t SD_getFileCount(File dir, uint8_t* fnum);

#endif