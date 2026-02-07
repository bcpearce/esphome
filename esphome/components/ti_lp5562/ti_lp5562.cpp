#include "esphome/components/ti_lp5562/ti_lp5562.h"

static constexpr uint8_t SETUP_ADDR = 0x00;
static constexpr uint8_t I2C_CTRL = 0x60;

static constexpr uint8_t CONFIG_ADDR = 0x08;
static constexpr uint8_t INTERNAL_CLK = 0x01;

static constexpr uint8_t LEDMAP_ADDR = 0x70;
static constexpr uint8_t LEDMAP_WONLY = 0x80;

static constexpr const char *FAIL_MSG{"Failed to reset LP5562 controller"};

namespace esphome::ti_lp5562 {
void TiLP5562LightOutput ::setup() {
  if (!this->write_byte(SETUP_ADDR, I2C_CTRL)) {
    this->mark_failed(LOG_STR(FAIL_MSG));
  }
  if (!this->write_byte(CONFIG_ADDR, INTERNAL_CLK)) {
    this->mark_failed(LOG_STR(FAIL_MSG));
  }
  if (!this->write_byte(LEDMAP_ADDR, LEDMAP_WONLY)) {
    this->mark_failed(LOG_STR(FAIL_MSG));
  }
  this->init_time_ = millis();
}
void TiLP5562LightOutput ::loop() {
  // Wait at least 500ms
  if (millis() - this->init_time_ > 500) {
    this->did_setup_ = true;
  }
}
void TiLP5562LightOutput ::write_state(light::LightState *state) {
  float red{0.f};
  float green{0.f};
  float blue{0.f};
  float white{0.f};
  state->current_values_as_rgbw(&red, &green, &blue, &white);
  if (this->did_setup_) {
    // If setup is complete, forward the setting to each channel for each change
    if (const uint8_t r = static_cast<uint8_t>(red * 0xFF); r != this->r_pwm_) {
      static constexpr uint8_t red_addr = 0x04;
      if (this->write_byte(red_addr, r)) {
        this->r_pwm_ = r;
      }
    }
    if (const uint8_t g = static_cast<uint8_t>(green * 0xFF); g != this->g_pwm_) {
      static constexpr uint8_t green_addr = 0x03;
      if (this->write_byte(green_addr, g)) {
        g_pwm_ = g;
      }
    }
    if (const uint8_t b = static_cast<uint8_t>(blue * 0xFF); b != this->b_pwm_) {
      static constexpr uint8_t blue_addr = 0x02;
      if (this->write_byte(blue_addr, b)) {
        this->b_pwm_ = b;
      }
    }
    if (const uint8_t w = static_cast<uint8_t>(white * 0xFF); w != this->w_pwm_) {
      static constexpr uint8_t white_addr = 0x0E;
      if (this->write_byte(white_addr, w)) {
        this->w_pwm_ = w;
      }
    }
  }
}
}  // namespace esphome::ti_lp5562
