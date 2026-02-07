#include "esphome/components/ti_lp5562/ti_lp5562.h"

static constexpr uint8_t SETUP_ADDR = 0x00;
static constexpr uint8_t I2C_CTRL = 0x60;

static constexpr uint8_t CONFIG_ADDR = 0x08;
static constexpr uint8_t INTERNAL_CLK = 0x01;

static constexpr uint8_t LEDMAP_ADDR = 0x70;
static constexpr uint8_t LEDMAP_WONLY = 0x80;

namespace esphome::ti_lp5562 {
void TiLP5562::setup() {
  if (!this->write_byte(SETUP_ADDR, I2C_CTRL)) {
    this->mark_failed("Failed to reset LP5562 controller");
  }
  if (!this->write_byte(CONFIG_ADDR, INTERNAL_CLK)) {
    this->mark_failed("Failed to enable LP5562 controller");
  }
  this->init_time_ = millis();
}
void TiLP5562::loop() {
  // Wait at least 500ms
  if (millis() - this->init_time_ > 500) {
    this->did_setup_ = true;
  }
}
void TiLP5562::write_state(light::LightState *state) {
  float red, green, blue, white;
  state->current_values_as_rgbw(&red, &green, &blue, &white, this->color_interlock_);
  if (this->did_setup_) {
    // If setup is complete, forward the setting to each channel for each change
    if (const uint8_t r = static_cast<uint8_t>(red * 0xFF); r != r_pwm_) {
      static constexpr uint8_t red_addr = 0x04;
      if (this->write_byte(red_addr, r)) {
        r_pwm_ = r;
      }
    }
    if (const uint8_t g = static_cast<uint8_t>(green * 0xFF); g != g_pwm_) {
      static constexpr uint8_t green_addr = 0x03;
      if (this->write_byte(green_addr, g)) {
        g_pwm_ = g;
      }
    }
    if (const uint8_t b = static_cast<uint8_t>(blue * 0xFF); b != b_pwm_) {
      static constexpr uint8_t blue_addr = 0x02;
      if (this->write_byte(blue_addr, b)) {
        b_pwm_ = b;
      }
    }
    if (const uint8_t w = static_cast<uint8_t>(white * 0xFF); w != w_pwm_) {
      static constexpr uint8_t white_addr = 0x0E;
      if (this->write_byte(white_addr, w)) {
        w_pwm_ = w;
      }
    }
  }
  this->red_->set_level(red);
  this->green_->set_level(green);
  this->blue_->set_level(blue);
  this->white_->set_level(white);
}
}  // namespace esphome::ti_lp5562
