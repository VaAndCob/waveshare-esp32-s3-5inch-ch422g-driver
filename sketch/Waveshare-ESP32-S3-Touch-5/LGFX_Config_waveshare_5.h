//Waveshare ESP32-S3 5" Touch screen
//https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-5#Schematic_diagram
//https://www.waveshare.com/esp32-s3-touch-lcd-5.htm?sku=28117


#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>

#define TFT_HOR_RES 1024
#define TFT_VER_RES 600

#define TOUCH_SDA 8
#define TOUCH_SCL 9
#define TOUCH_INT 4
#define TOUCH_RST -1

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
/*
#define DI0 0 //digital input common ground OPEN-HIGH, CLOSE-LOW 
#define TP_RST 1 //touch reset
#define LCD_BL 2  //backlight
#define LCD_RST 3 //reset lcd
#define SD_CS 4   //sd select
#define DI1 5  //digital input common ground OPEN-HIGH, CLOSE-LOW 

#define DO0 0 //digital output -|LOAD|+ 5-36V|GND (LOW ON, HIGH OFF)
#define DO1 1


// CH422G I2C device addresses (сдвинутые!)
#define CH422G_ADDR_WR_SET 0x24
#define CH422G_ADDR_WR_IO 0x38
#define CH422G_ADDR_RD_IO 0x26
#define CH422G_ADDR_WR_OC 0x23


inline void initCH422G() {
  Wire.begin(I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
  uint8_t data = 0x81;
  Wire.beginTransmission(CH422G_ADDR_WR_SET);
  Wire.write(data);
  Wire.endTransmission();
  uint8_t io_state = 0xFF;
  Wire.beginTransmission(CH422G_ADDR_WR_IO);
  Wire.write(io_state);
  Wire.endTransmission();
  delay(20);
  Wire.end();
  delay(100);
}
*/
// ---------------------- Backlight via CH422G ----------------------
/*
class IoExpandedBacklight : public lgfx::ILight {
public:
  void config(lgfx::ITouch::config_t *cfg) {
    if (cfg) {
      _cfg = *cfg;
    }
  }

  bool init(uint8_t brightness) override {

    return true;
  }

  void setBrightness(uint8_t brightness) override {
    uint8_t io_state = (brightness == 0) ? 0xFB : 0xFF;  // 0 -> (11111011) else (11111111)
    lgfx::i2c::transactionWrite(_cfg.i2c_port, CH422G_ADDR_WR_IO, &io_state, 1, _cfg.freq);
  }

private:
  lgfx::ITouch::config_t _cfg{};
};
*/
// -------------------------------------------------------------------

class LGFX : public lgfx::LGFX_Device {
public:
  lgfx::Bus_RGB _bus_instance;
  lgfx::Panel_RGB _panel_instance;
//  IoExpandedBacklight _light_instance;
  lgfx::Touch_GT911 _touch_instance;

  LGFX(void) {
    {
      auto cfg = _panel_instance.config();
      cfg.memory_width = TFT_HOR_RES;
      cfg.memory_height = TFT_VER_RES;
      cfg.panel_width = TFT_HOR_RES;
      cfg.panel_height = TFT_VER_RES;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }

    {
      auto cfg = _panel_instance.config_detail();
      cfg.use_psram = 1;
      _panel_instance.config_detail(cfg);
    }


    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;

      cfg.pin_d0 = 14;  // B0
      cfg.pin_d1 = 38;  // B1
      cfg.pin_d2 = 18;  // B2
      cfg.pin_d3 = 17;  // B3
      cfg.pin_d4 = 10;  // B4

      cfg.pin_d5 = 39;   // G0
      cfg.pin_d6 = 0;    // G1
      cfg.pin_d7 = 45;   // G2
      cfg.pin_d8 = 48;   // G3
      cfg.pin_d9 = 47;   // G4
      cfg.pin_d10 = 21;  // G5

      cfg.pin_d11 = 1;   // R0
      cfg.pin_d12 = 2;   // R1
      cfg.pin_d13 = 42;  // R2
      cfg.pin_d14 = 41;  // R3
      cfg.pin_d15 = 40;  // R4

      cfg.pin_henable = 5;
      cfg.pin_vsync = 3;
      cfg.pin_hsync = 46;
      cfg.pin_pclk = 7;

      cfg.freq_write = 14000000;  //8 Mhz

      cfg.hsync_polarity = 0;
      cfg.hsync_front_porch = 40;  //20
      cfg.hsync_pulse_width = 20;  //10
      cfg.hsync_back_porch = 40;   //20


      cfg.vsync_polarity = 0;
      cfg.vsync_front_porch = 40;  //20
      cfg.vsync_pulse_width = 20;  //10
      cfg.vsync_back_porch = 40;   //20

      cfg.pclk_active_neg = 0;
      cfg.de_idle_high = 0;
      cfg.pclk_idle_high = 0;

      _bus_instance.config(cfg);
    }

    _panel_instance.setBus(&_bus_instance);



    {
      auto cfg = _touch_instance.config();
      cfg.x_min = 0;
      cfg.x_max = TFT_HOR_RES - 1;
      cfg.y_min = 0;
      cfg.y_max = TFT_VER_RES - 1;
      cfg.pin_int = TOUCH_INT;

      cfg.pin_rst = TOUCH_RST;
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;
      cfg.i2c_port = 1;

      cfg.pin_sda = TOUCH_SDA;
      cfg.pin_scl = TOUCH_SCL;
      cfg.freq = 400000;

      cfg.i2c_addr = 0x14;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
/*
    {
      auto touch_cfg = _touch_instance.config();
      _light_instance.config(&touch_cfg);
      _panel_instance.setLight(&_light_instance);
    }
*/
    setPanel(&_panel_instance);
  }
};