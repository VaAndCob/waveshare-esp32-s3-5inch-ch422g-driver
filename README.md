# Waveshare 5" ESP32-S3 Touch LCD Demo with Custom CH422G Driver

This repository contains a demonstration project for the [Waveshare 5" ESP32-S3 Touch LCD](https://www.waveshare.com/esp32-s3-touch-lcd-5.htm), showcasing a custom, lightweight driver for the onboard **CH422G I/O expander**.

The primary goal of this project was to create a simple and reliable way to control the board's peripherals (Backlight, Digital I/O, SD Card) without relying on external libraries like `esp32_IO_expander`, which can be complex to integrate. This driver is written from scratch and uses the LovyanGFX I2C bus to ensure compatibility and prevent conflicts with the touch controller.

## Topics
`esp32-s3` `waveshare` `ch422g` `lovyanGFX` `lvgl` `squareline-studio` `touch-screen` `custom-driver` `arduino`

## 🎞️[Youtube](https://youtu.be/34sBNREli5g)
## Project UI Screenshot
![Project UI Screenshot](https://github.com/VaAndCob/waveshare-esp32-s3-5inch-ch422g-driver/blob/main/document/20251018_200916.jpg)


## The Problem
The Waveshare 5" board uses a CH422G chip to expand the I/O capabilities of the ESP32-S3. This expander controls crucial functions like the LCD backlight, digital inputs/outputs, and the SD card's chip select line. The main challenge is that both the CH422G and the capacitive touch controller (GT911) share the **same I2C bus**.

Using separate libraries or the standard Arduino `Wire` library for the CH422G alongside LovyanGFX's touch driver can lead to I2C bus contention, race conditions, and unpredictable behavior. The system can become unstable as two different parts of the code try to manage the same hardware bus.

This project solves that problem by implementing a custom CH422G driver that **exclusively uses the I2C bus managed by LovyanGFX**. This ensures all I2C communication is properly arbitrated, preventing conflicts and creating a stable, reliable system.

## Key Features

*   **Custom CH422G Driver:** A minimal, from-scratch driver (`waveshare_ch422g.cpp`) to directly control the I/O expander.
*   **Conflict-Free I2C:** All communication with the CH422G is routed through LovyanGFX's I2C bus manager (`lgfx::i2c`). This is the key to preventing conflicts with the touch driver, which also operates on the same bus.
*   **Full Peripheral Control:**
    *   LCD Backlight (On/Off)
    *   2x Digital Outputs (DO0, DO1)
    *   2x Digital Inputs (DI0, DI1)
    *   SD Card Chip Select (CS) for the integrated card slot.
*   **LVGL User Interface:** A modern UI built with SquareLine Studio to demonstrate the functionality.
*   **LovyanGFX Powered:** Utilizes the powerful LovyanGFX library for display driving.
*   **SD Card Support:** Includes helper functions for initializing the SD card and listing files.

## Hardware & Pinout

This project is designed for the Waveshare 5" ESP32-S3 Touch LCD. The CH422G I/O expander is mapped as follows:

*   `EXIO0` - **DI0** (Digital Input 0)
*   `EXIO1` - **TP_RST** (Touch Panel Reset)
*   `EXIO2` - **LCD_BL** (LCD Backlight Control)
*   `EXIO3` - **LCD_RST** (LCD Reset)
*   `EXIO4` - **SD_CS** (SD Card Chip Select)
*   `EXIO5` - **DI1** (Digital Input 1)
*   `EXIO6` - Not Used
*   `EXIO7` - Not Used
*   `OC0` - **DO0** (Digital Output 0)
*   `OC1` - **DO1** (Digital Output 1)
*   `OC2` - Not Used
*   `OC3` - Not Used

The SD Card uses the following SPI pins, with the Chip Select being controlled via the CH422G:

*   `MOSI`: GPIO 11
*   `MISO`: GPIO 13
*   `CLK`:  GPIO 12
*   `CS`:   GPIO 4 on CH422G (`EXIO4`)

## 3D Printed Case

A custom-designed 3D printable case is available for this board. This case provides protection and a clean, finished look for your project.

[Download the 3D model from MakerWorld](https://makerworld.com/en/models/1879657-waveshare-esp32-s3-5-touchscreen-board-case#profileId-2012633)


## Software & Libraries

*   **Framework:** Arduino
*   **IDE:** Arduino IDE or PlatformIO
*   **Libraries:**
    *   LovyanGFX (must be installed)
    *   LVGL 8.3.11 (included, managed by SquareLine Studio)
    *   SPI, SD (built-in Arduino libraries)

## How to Get Started

1.  **Clone the Repository:**
    ```sh
    git clone https://github.com/VaAndCob/waveshare-esp32-s3-5inch-ch422g-driver
    ```

2.  **Install Libraries:**
    *   Open the Arduino IDE.
    *   Go to `Sketch` -> `Include Library` -> `Manage Libraries...`.
    *   Search for and install `LovyanGFX`.

3.  **Configure LVGL:**
    *   You must copy this `lv_conf.h` file and place it in your main Arduino `libraries` folder. It should sit alongside the `lvgl` library folder, not inside it. The path is typically `.../Arduino/Sketch/libraries/`.

3.  **Configure Board:**
    *  In the Arduino IDE, select a board definition for the ESP32-S3 (e.g., "ESP32-S3-Dev-Module").
    * Hardware: ESP32-S3 based + SDReader + PSRAM + 5' TFT (1024x600) with Capacitive touch screen https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-5
    * IDE: Arduino IDE 2.3.6
    * ESP32 Core: 2.0.17
    * PSRAM: OPI PSRAM
    * Flash Szie: 16 MB
    * Partition scheme: 16 MB (3MB) with SPIFFS

4.  **Open and Upload:**
    *   Open the main sketch file: `sketch/Waveshare-ESP32-S3-Touch-5/Waveshare-ESP32-S3-Touch-5.ino`.
    *   Compile and upload the project to your board.

## Project Structure

```
├── README.md
└── sketch/Waveshare-ESP32-S3-Touch-5/
    ├── Waveshare-ESP32-S3-Touch-5.ino  # Main Arduino sketch, setup and loop
    ├── LGFX_ESP32_S3_5inch_800x480.h    # LovyanGFX board configuration
    ├── waveshare_ch422g.h/.cpp         # The custom CH422G I/O expander driver
    ├── waveshare_sd_card.h/.cpp        # Helper functions for the SD card
    ├── ui.h/.c                         # LVGL UI files generated by SquareLine Studio
    ├── ui_events.h/.cpp                # UI event handlers (your custom logic)
    └── ... (other UI files)
```

*   **`waveshare_ch422g.cpp`**: This is the heart of the project. It contains low-level I2C functions to configure the CH422G and control its pins.
*   **`ui_events.cpp`**: This is where the UI widgets (switches, buttons) are linked to the driver functions. For example, toggling the `DO0` switch calls the `DO_Write()` function.

## License

This project is open-source and available under the MIT License.

---

*This project was created to simplify development on the Waveshare ESP32-S3 5" display and to share a working solution with the community. Contributions and feedback are welcome!*
