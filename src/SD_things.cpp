#include "SD_things.h"

#include "DEBUG_things.h"

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

    char file_name[16];
    f.getName(file_name, 16);

    if (strcmp(file_name, "System Volume I")) {  // debe haber una mejor manera
                                                 // de evitar que pase esto
      if (i == fnum) {
        strcpy(buffer, file_name);

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

    char file_name[16];
    f.getName(file_name, 16);

    if (strcmp(file_name, "System Volume I")) {
      i++;
    }
  }
  return -1;
}
