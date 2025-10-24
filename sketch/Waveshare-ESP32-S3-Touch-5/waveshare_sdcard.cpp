// Library for SD Card


#include "ui.h"
#include "waveshare_ch422g.h"
/*
 * Connect the SD card to the following pins:
 *
 * SD Card |   ESP32
 *    D2       - (Not connected)
 *    D3       SS (Chip Select)
 *    CMD      MOSI (Master output slave input)
 *    VSS      GND (Ground)
 *    VDD      3.3V (Power supply)
 *    CLK      SCK (Clock)
 *    VSS      GND (Ground)
 *    D0       MISO (Master Input Slave Output)
 *    D1       - (Not connected)
 */

#include "waveshare_sdcard.h"


void initSDCARD() {
    Serial.println("Initialize SD Card");
  // Initialize SPI
   // EX_Write(SD_CS, HIGH);//disable SD CARD
    EX_Write(SD_CS, LOW); //enable sd card
    SPI.setHwCs(false);
    SPI.begin(SD_CLK, SD_MISO, SD_MOSI, -1);

    if (!SD.begin(SD_FAKE_CS,SPI)) { // Passes 4 as the CS index
         lv_textarea_add_text(ui_TextArea1,"\nCard Mount Failed\n"); 
        return;
    }
     lv_textarea_add_text(ui_TextArea1,"\nSD Card Mounted successfully\n");
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        lv_textarea_add_text(ui_TextArea1,"\nNo SD card attached\n"); // No SD card connected
        return;
    }

 
    if (cardType == CARD_MMC) {
         lv_textarea_add_text(ui_TextArea1,"SD Card Type: MMC\n");
    } else if (cardType == CARD_SD) {
         lv_textarea_add_text(ui_TextArea1,"SD Card Type: SDSC\n");
    } else if (cardType == CARD_SDHC) {
         lv_textarea_add_text(ui_TextArea1,"SD Card Type: SDHC\n");
    } else {
         lv_textarea_add_text(ui_TextArea1,"SD Card Type: UNKNOWN\n"); // Unknown Type
    }


}

// List files and subdirectories in a directory
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
    String txt = "Listing directory: "+ String(dirname) + "\n";
    lv_textarea_add_text(ui_TextArea1, txt.c_str());
 
    // Listing Directories
    File root = fs.open(dirname);
    if (!root) {
        lv_textarea_add_text(ui_TextArea1,"Failed to open directory\n");
        return;
    }
    if (!root.isDirectory()) {
        lv_textarea_add_text(ui_TextArea1,"Not a directory\n");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            txt = String(" DIR : ") + String(file.name()) + "\n";
              lv_textarea_add_text(ui_TextArea1,txt.c_str());
            if (levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            txt = String("  FILE: ") + String(file.name()) + " SIZE: "+ String(file.size())+"\n";
              lv_textarea_add_text(ui_TextArea1,txt.c_str());
        }
        file = root.openNextFile();
    }
    
}

// Creating Dir
void createDir(fs::FS &fs, const char * path) {
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path)) {
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

// Remove a directory
void removeDir(fs::FS &fs, const char * path) {
    Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path)) {
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

// Reading a file
void readFile(fs::FS &fs, const char * path) {
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

// Writing to a File
void writeFile(fs::FS &fs, const char * path, const char * message) {
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

// Append to file
void appendFile(fs::FS &fs, const char * path, const char * message) {
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message)) {
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

// Rename a file
void renameFile(fs::FS &fs, const char * path1, const char * path2) {
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

// Deleting files
void deleteFile(fs::FS &fs, const char * path) {
    Serial.printf("Deleting file: %s\n", path);
    if (fs.remove(path)) {
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

// Test file IO performance
void testFileIO(fs::FS &fs, const char * path) {
    File file = fs.open(path);
    static uint8_t buf[512];  // Buffer
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file) {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len) {
            size_t toRead = len;
            if (toRead > 512) {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf(" %u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }

    // Write test
    file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++) {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
