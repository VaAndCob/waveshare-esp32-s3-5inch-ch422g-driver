/* An Example Code Template for Waveshare ESP32-S3 Touch 5" that fully work with CH422G expansion

 * @file Waveshare ESP32-S3 Touch 5"
 * @author Va&Cob
 * @date 2025-10-01
 * @copyright Copyright (c) 2025 Va&Cob
 * ---------------------------------------------------
 * Hardware: ESP32-S3 based + SDReader + PSRAM + 5' TFT (1024x600) with Capacitive touch screen https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-5
 * IDE: Arduino IDE 2.3.6
 * ESP32 Core: 2.0.17
 * PSRAM: OPI PSRAM
 * Flash Szie: 16 MB
 * Partition scheme: 16 MB (3MB) with SPIFFS

   REMARK: copy file "src/lv_conf.h"  into "Arduino/sketch/libraries"
-------------------------------------------------------------
*/

#include <Arduino.h>

#include <LovyanGFX.hpp>
#include "LGFX_Config_waveshare_5.h"
LGFX lcd;
#include "waveshare_ch422g.h"
#include "waveshare_sd_card.h"

#include <lvgl.h>
#include "ui.h"
#include "ui_events.h"

//-----------------------------------------
// my_disp_flush - Option A (async, minimal)
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  lcd.pushImageDMA(area->x1, area->y1, w, h, (lgfx::rgb565_t *)&color_p->full);
  lv_disp_flush_ready(disp);
}

//touchpad callback
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {

  uint16_t touchX, touchY;
  bool touched = lcd.getTouch(&touchX, &touchY);
  // Serial.printf("getTouch got=%d %d,%d\n", touched, touchX, touchY);

  if (touched) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}


//------------

unsigned long loopnow = millis();
//---------------------------

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Waveshare ESP32-S3 Touch 5\" Test");
  // initCH422G();

  lcd.init();
  lcd.setRotation(0);

  lv_init();

  // LVGL display buffers
  const uint16_t screenWidth = lcd.width();
  const uint16_t screenHeight = lcd.height();

  static lv_disp_draw_buf_t draw_buf;
#define LVGL_BUFFER_LINES 120

  lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(
    TFT_HOR_RES * LVGL_BUFFER_LINES * sizeof(lv_color_t),
    MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);

  lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(
    TFT_HOR_RES * LVGL_BUFFER_LINES * sizeof(lv_color_t),
    MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);

  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, TFT_HOR_RES * LVGL_BUFFER_LINES);


  // Register display driver
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.full_refresh = 0;  // ❌ DO NOT use full_refresh=1 unless required
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);


  // Touch input driver
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);


  ui_init();
  initSDCARD();
}
//--------------------------------------------
void loop() {
  lv_task_handler();

  //Read input terminal
  if (EX_Read(DI0)) {
    if (!DI0_shadow_state) {
      lv_obj_set_style_bg_color(ui_DI0, lv_color_hex(0x000000), LV_PART_MAIN);
      DI0_shadow_state = HIGH;
    }
  } else {
    if (DI0_shadow_state) {
      lv_obj_set_style_bg_color(ui_DI0, lv_color_hex(0x0000FF), LV_PART_MAIN);
      DI0_shadow_state = LOW;
    }
  }

  if (EX_Read(DI1)) {
    if (!DI1_shadow_state) {
      lv_obj_set_style_bg_color(ui_DI1, lv_color_hex(0x000000), LV_PART_MAIN);
      DI1_shadow_state = HIGH;
    }
  } else {
    if (DI1_shadow_state) {
      lv_obj_set_style_bg_color(ui_DI1, lv_color_hex(0x0000FF), LV_PART_MAIN);
      DI1_shadow_state = LOW;
    }
  }
}
//----------------------------------------------------