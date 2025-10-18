//CH422G IO control function that work under the same I2C bus of lovyanGFX


#include "waveshare_ch422g.h"

#define I2C_PORT 1
#define I2C_FREQ 400000

uint8_t EXIO_state = 0b11111111;
uint8_t DO_state = 0b00000000;
uint8_t DI0_shadow_state = 0;
uint8_t DI1_shadow_state = 0;

/* WR_SET address bit function 
bit 7 -> 1 = sleep mode, 0 = all enable
bit 1 ->  DO Output (DO0 - DO3) 1 = enable , 0 = disable
bit 0 ->  Control EXIO  1 = write , 0 = read

*/
#define CH422G_CONFIG_WRITE_ENABLE 0b00000001
#define CH422G_CONFIG_READ_ENABLE  0b00000010
//-------------------------------------------------------
// force initialize CH422G INPUT and OUTPUT
void initCH422G() {
  Wire.begin(I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);

  // OC active + EXIO readable
  uint8_t config = CH422G_CONFIG_WRITE_ENABLE;
  //0x81 0b10000001     EX_Write OK  | EX_Read FAIL | DO_Write FAIL
  //0x82 0b10000010     EX_Write FAIL | EX_Read OK  | DO_Write OK
  Wire.beginTransmission(CH422G_ADDR_WR_SET);
  Wire.write(config);
  Wire.endTransmission();
  delay(20);

  // 2️⃣ Configure EXIO: 1 = input/high, 0 = output-low
  // DI0, DI1 = inputs => bits 0 and 5 = 1
  // Others = outputs default high (1) except if you want to drive low at init
  // Start all HIGH (safe default)
  Wire.beginTransmission(CH422G_ADDR_WR_IO);
  Wire.write(EXIO_state);
  Wire.endTransmission();
  delay(20);

  // 3️⃣ Initialize OC lines (all off)
  uint8_t oc_state = 0x0F;  // OC0–OC3 off (open)
  Wire.beginTransmission(CH422G_ADDR_WR_OC);
  Wire.write(oc_state);
  Wire.endTransmission();
  delay(20);

  Wire.end();
  delay(100);
}

//---------- Digital OUTPUT --------------------

// set digital output port DO0 and DO1
void DO_Write(uint8_t pin, bool state) {
  if (state)
    DO_state |= (1 << pin);  // set bit
  else
    DO_state &= ~(1 << pin);                                                         // clear bit
  lgfx::i2c::transactionWrite(I2C_PORT, CH422G_ADDR_WR_OC, &DO_state, 1, I2C_FREQ);  // Write DO
  delay(5);
}

// Read DO state
uint8_t DO_Read(uint8_t pin) {
  return (DO_state >> pin) & 0x01;
}
//------------------------------------
// Read specific EXIO pin
bool EX_Read(uint8_t pin) {
  
// 1. Configure the chip to READ mode (0b00000010)
    uint8_t read_config = CH422G_CONFIG_READ_ENABLE;
    // Use the variable's address (&read_config) and the length (1)
    auto cres = lgfx::i2c::transactionWrite(I2C_PORT, CH422G_ADDR_WR_SET, &read_config, (uint8_t)1, I2C_FREQ);
    if (!cres) {
        Serial.println("EX Read: Config set failed");
    }

    // 2. Perform the read transaction
    uint8_t inputs;
    auto res = lgfx::v1::i2c::transactionRead(I2C_PORT, CH422G_ADDR_RD_IO, &inputs, (uint8_t)1, I2C_FREQ);
    if (!res) return false; 

    // 3. CRITICAL: Restore the configuration back to WRITE mode (0b00000001) immediately
    uint8_t write_config = CH422G_CONFIG_WRITE_ENABLE;
    lgfx::i2c::transactionWrite(I2C_PORT, CH422G_ADDR_WR_SET, &write_config, (uint8_t)1, I2C_FREQ);
    
    // 4. Return the state of the requested pin
    return (inputs >> pin) & 0x01;
}
//---------------------------
// EXIO OUTPUT  (EX0–EX7)

// set digital output port DO0 and DO1
// Inside EX_Write(uint8_t pin, bool state) { ... }

void EX_Write(uint8_t pin, bool state) {

  // 1. CRITICAL: Enable CH422G Write mode (0x81)
  uint8_t write_config = CH422G_CONFIG_WRITE_ENABLE;

  // Use the variable's address (&write_config) and the length (1)
  auto cres = lgfx::i2c::transactionWrite(I2C_PORT, CH422G_ADDR_WR_SET, &write_config, (uint8_t)1, I2C_FREQ);
  if (!cres) {
    Serial.println("EX Write: Config set failed");
  }

  // 2. Modify the local shadow state
  if (state)
    EXIO_state |= (1 << pin);
  else
    EXIO_state &= ~(1 << pin);

  // 3. Write the full shadow state to the CH422G WR_IO register (0x38)
  auto wres = lgfx::i2c::transactionWrite(I2C_PORT, CH422G_ADDR_WR_IO, &EXIO_state, (uint8_t)1, I2C_FREQ);
  if (!wres) Serial.println("EX Write: Data write failed");
}
//---------------------------------------------