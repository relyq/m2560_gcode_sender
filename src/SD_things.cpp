#include "SD_things.h"

#include "DEBUG_things.h"

int8_t SD_filecount(File dir, uint8_t* filecount, char* filenames[]) {
  if (!dir || !filecount || !filenames) {
    return -1;
    DEBUG_PRINTLN("filecount error");
  }

  uint8_t i = 0;
  while (1) {
    File f = dir.openNextFile();

    if (!f) {
      // no more files
      *filecount = i;
      dir.rewindDirectory();
      return 0;
    }
    if (strcmp(f.name(), "SYSTEM~1")) {
      DEBUG_PRINT(i);
      DEBUG_PRINT(".\t");

      DEBUG_PRINT(f.name());
      strcpy(filenames[i], f.name());
      // filenames[i] = f.name();

      DEBUG_PRINT("\t\t");
      DEBUG_PRINTLNDEC(f.size());
      DEBUG_PRINTLN(filenames[i]);
      i++;
    }
  }
  return -1;
}

int8_t SD_getFileName(File dir, uint8_t fnum, char* buffer) {
  if (!dir || !buffer) {
    return -1;
    DEBUG_PRINTLN("filecount error");
  }

  uint8_t i = 0;
  while (1) {
    File f = dir.openNextFile();

    if (!f) {
      // no more files - file not found
      dir.rewindDirectory();
      DEBUG_PRINTLN("file not found");
      return 1;
    }
    if (strcmp(f.name(), "SYSTEM~1")) {
      if (i == fnum) {
        strcpy(buffer, f.name());

        dir.rewindDirectory();
        return 0;
      }
      i++;
    }
  }
  return -1;
}

int8_t SD_getFileCount(File dir, uint8_t* fnum) {
  if (!dir || !fnum) {
    return -1;
    DEBUG_PRINTLN("filecount error");
  }

  uint8_t i = 0;
  while (1) {
    File f = dir.openNextFile();

    if (!f) {
      // no more files - file not found
      dir.rewindDirectory();
      *fnum = i;
      return 0;
    }
    if (strcmp(f.name(), "SYSTEM~1")) {
      i++;
    }
  }
  return -1;
}
