#pragma once

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_MOSI 11    // SD card master output slave input pin
#define SD_CLK  12    // SD card clock pin
#define SD_MISO 13    // SD card master input slave output pin
#define SD_FAKE_CS 6 // fake ESP32 pin to pass to the library

void initSDCARD();
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void readFile(fs::FS &fs, const char * path);
void deleteFile(fs::FS &fs, const char * path);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void testFileIO(fs::FS &fs, const char * path);