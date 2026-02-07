#include "esphome/components/ti_lp5562/ti_lp5562.h"

static constexpr uint8_t SETUP_ADDR = 0x00;
static constexpr uint8_t I2C_CTRL = 0x60;

static constexpr uint8_t CONFIG_ADDR = 0x08;
static constexpr uint8_t INTERNAL_CLK = 0x01;

static constexpr uint8_t LEDMAP_ADDR = 0x70;
static constexpr uint8_t LEDMAP_RGBW = 0x00;

static constexpr const char *FAIL_MSG{"Failed to reset LP5562 controller"};

static constexpr const char *TAG{"ti_lp5562"};

namespace esphome::ti_lp5562 {
void TiLP5562LightOutput ::setup() {
  if (!this->write_byte(SETUP_ADDR, I2C_CTRL)) {
    esph_log_d(TAG, "Reset LP5562 Controller");
    this->mark_failed(LOG_STR(FAIL_MSG));
  }
  this->init_time_ = millis();
}
void TiLP5562LightOutput ::loop() {
  // Wait at least 500ms
  if (millis() - this->init_time_ > 500 && !this->did_setup_) {
    esph_log_d(TAG, "Configuring LP5562 Controller");
    if (!this->write_byte(CONFIG_ADDR, INTERNAL_CLK)) {
      this->mark_failed(LOG_STR(FAIL_MSG));
      return;
    }
    if (!this->write_byte(LEDMAP_ADDR, LEDMAP_RGBW)) {
      this->mark_failed(LOG_STR(FAIL_MSG));
      return;
    }
    this->did_setup_ = true;
    esph_log_d(TAG, "Startup complete");
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
        esph_log_d(TAG, "Set Red to %.0f%% duty cycle (0x%02x)", red, r);
      }
    }
    if (const uint8_t g = static_cast<uint8_t>(green * 0xFF); g != this->g_pwm_) {
      static constexpr uint8_t green_addr = 0x03;
      if (this->write_byte(green_addr, g)) {
        this->g_pwm_ = g;
        esph_log_d(TAG, "Set Green to %.0f%% duty cycle (0x%02x)", green, g);
      }
    }
    if (const uint8_t b = static_cast<uint8_t>(blue * 0xFF); b != this->b_pwm_) {
      static constexpr uint8_t blue_addr = 0x02;
      if (this->write_byte(blue_addr, b)) {
        this->b_pwm_ = b;
        esph_log_d(TAG, "Set Blue to %.0f%% duty cycle (0x%02x)", blue, b);
      }
    }
    if (const uint8_t w = static_cast<uint8_t>(white * 0xFF); w != this->w_pwm_) {
      static constexpr uint8_t white_addr = 0x0E;
      if (this->write_byte(white_addr, w)) {
        this->w_pwm_ = w;
        esph_log_d(TAG, "Set White to %.0f%% duty cycle (0x%02x)", white, w);
      }
    }
  }
}

light::LightTraits TiLP5562LightOutput::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supported_color_modes({this->mode_});
  return traits;
}

void TiLP5562LightOutput::dump_config() {
  ESP_LOGCONFIG(TAG, "LP5562 RGBW LED Controller:");
  ESP_LOGCONFIG(TAG, "  Color Mode: %d", this->mode_);
  switch (this->mode_) {
    case light::ColorMode::RGB:
    case light::ColorMode::RGB_WHITE:
      ESP_LOGCONFIG(TAG, "  R: duty (0x%02x)", this->r_pwm_);
      ESP_LOGCONFIG(TAG, "  G: duty (0x%02x)", this->g_pwm_);
      ESP_LOGCONFIG(TAG, "  B: duty (0x%02x)", this->b_pwm_);
      break;
  }
  switch (this->mode_) {
    case light::ColorMode::WHITE:
    case light::ColorMode::BRIGHTNESS:
      ESP_LOGCONFIG(TAG, "  W: duty (0x%02x)", this->w_pwm_);
      break;
  }
}

}  // namespace esphome::ti_lp5562
