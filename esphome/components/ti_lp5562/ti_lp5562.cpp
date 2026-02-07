#include "esphome/components/ti_lp5562/ti_lp5562.h"

namespace esphome::ti_lp5562 {

static constexpr uint8_t SETUP_ADDR = 0x00;
static constexpr uint8_t I2C_CTRL = 0x60;

static constexpr uint8_t CONFIG_ADDR = 0x08;
static constexpr uint8_t INTERNAL_CLK = 0x01;

static constexpr uint8_t LEDMAP_ADDR = 0x70;
static constexpr uint8_t LEDMAP_RGBW = 0x00;

static constexpr const char *FAIL_MSG{"Failed to reset LP5562 controller"};

static constexpr const char *TAG{"ti_lp5562"};

static const char *get_channel_name(ChannelAddr channel) {
  switch (channel) {
    case ChannelAddr::RED:
      return "red";
    case ChannelAddr::GREEN:
      return "green";
    case ChannelAddr::BLUE:
      return "blue";
    case ChannelAddr::WHITE:
      return "white";
  }
}

void TiLP5562LightOutput ::setup() {
  if (!this->write_byte(SETUP_ADDR, I2C_CTRL)) {
    this->mark_failed(LOG_STR(FAIL_MSG));
  }
  this->init_time_ = millis();
  esph_log_d(TAG, "Reset LP5562 Controller");
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
    _set_channel(red, this->r_duty_, ChannelAddr::RED);
    _set_channel(green, this->g_duty_, ChannelAddr::GREEN);
    _set_channel(blue, this->b_duty_, ChannelAddr::BLUE);
    _set_channel(white, this->w_duty_, ChannelAddr::WHITE);
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
      ESP_LOGCONFIG(TAG, "  R: duty (0x%02x)", this->r_duty_);
      ESP_LOGCONFIG(TAG, "  G: duty (0x%02x)", this->g_duty_);
      ESP_LOGCONFIG(TAG, "  B: duty (0x%02x)", this->b_duty_);
      break;
  }
  switch (this->mode_) {
    case light::ColorMode::WHITE:
    case light::ColorMode::BRIGHTNESS:
      ESP_LOGCONFIG(TAG, "  W: duty (0x%02x)", this->w_duty_);
      break;
  }
}

void TiLP5562LightOutput::_set_channel(float channel_state, uint8_t &duty, ChannelAddr channel_addr) {
  if (const uint8_t new_duty = static_cast<uint8_t>(channel_state * 0xFF); duty != duty) {
    if (this->write_byte(static_cast<uint8_t>(channel_addr), new_duty)) {
      duty = new_duty;
      esph_log_d(TAG, "Set %s to %.0f%% duty cycle (0x%02x)", get_channel_name(channel_addr), channel_state * 100.f,
                 new_duty);
    }
  }
}

}  // namespace esphome::ti_lp5562
