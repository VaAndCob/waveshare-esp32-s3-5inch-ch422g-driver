#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <LovyanGFX.hpp>

//-----------  MARCO ----------------------------
//OUTPUT and Backlight Control
#define DO0_ON \
  do { DO_Write(DO0, LOW); } while (0)
#define DO0_OFF \
  do { DO_Write(DO0, HIGH); } while (0)
#define DO1_ON \
  do { DO_Write(DO1, LOW); } while (0)
#define DO1_OFF \
  do { DO_Write(DO1, HIGH); } while (0)

#define BL_ON \
  do { EX_Write(LCD_BL, HIGH); } while (0)
#define BL_OFF \
  do { EX_Write(LCD_BL, LOW); } while (0)


//----------------------------------------------
/*CH422G Pin Map for Waveshare ESP32-S3 Touch 5"
EXIO0 - DI0
EXIO1 - TP_RST
EXI02 - LCD_BL
EXI03 - LCD_RESET
EXIO4 - SD_CS
EXIO5 - DI1
EXIO6 - NOT USE
EXIO7 - NOT USE
OC0 - DO0
OC1 - DO1 
OC2 - NOT USE
OC3 - NOT USE
*/

#define CAN_RX 19
#define CAN_TX 20


#define DI0 0      // INPUT common ground OPEN-HIGH, CLOSE-LOW
#define TP_RST 1   // OUTPUT touch reset
#define LCD_BL 2   // OUTPUT backlight
#define LCD_RST 3  // OUTPUT reset lcd
#define SD_CS 4    // OUTPUT sd select
#define DI1 5      // INPUT common ground OPEN-HIGH, CLOSE-LOW

#define DO0 0  //digital output -|LOAD|+ 5-36V|GND (LOW ON, HIGH OFF)
#define DO1 1

#define CH422G_CONFIG_WRITE_ENABLE 0b00000001
#define CH422G_CONFIG_READ_ENABLE  0b00000010
// CH422G I2C device addresses
#define CH422G_ADDR_WR_SET 0x24 
#define CH422G_ADDR_WR_IO  0x38
#define CH422G_ADDR_RD_IO  0x26
#define CH422G_ADDR_WR_OC  0x23

#define I2C_PORT 1
#define I2C_FREQ 400000
// I2C Pin define
#define I2C_MASTER_NUM 0
#define I2C_MASTER_SDA_IO 8
#define I2C_MASTER_SCL_IO 9

extern uint8_t DO_state;
extern uint8_t EXIO_state;
extern uint8_t DI0_shadow_state;
extern uint8_t DI1_shadow_state;


//void initCH422G();
void DO_Write(uint8_t pin, bool state);
uint8_t DO_Read(uint8_t pin);
void EX_Write(uint8_t pin, bool state);
bool EX_Read(uint8_t pin);
