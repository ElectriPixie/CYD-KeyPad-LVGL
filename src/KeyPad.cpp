#include <Arduino.h>
#include <esp32_smartdisplay.h>

void setup()
{
  smartdisplay_init();

  auto display = lv_display_get_default();
  // lv_display_set_rotation(display, LV_DISPLAY_ROTATION_90);
  // lv_display_set_rotation(display, LV_DISPLAY_ROTATION_180);
  // lv_display_set_rotation(display, LV_DISPLAY_ROTATION_270);
}

auto lv_last_tick = millis();

void loop()
{
    auto const now = millis();
    // Update the ticker
    lv_tick_inc(now - lv_last_tick);
    lv_last_tick = now;
    // Update the UI
    lv_timer_handler();
}