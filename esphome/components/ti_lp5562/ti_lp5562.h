#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ti_lp5562 {
enum class ChannelAddr : uint8_t { RED = 0x04, GREEN = 0x03, BLUE = 0x02, WHITE = 0x0E };

class TiLP5562LightOutput : public light::LightOutput, public i2c::I2CDevice, public Component {
 public:
  void set_color_mode(light::ColorMode mode) { mode_ = mode; };
  void setup() override;
  void loop() override;
  void dump_config();
  float get_setup_priority() const override { return esphome::setup_priority::DATA; }

  void setup_state(light::LightState *state) override { this->light_state_ = state; }
  void write_state(light::LightState *state) override;
  light::LightTraits get_traits() override;

 protected:
  light::ColorMode mode_{light::ColorMode::UNKNOWN};
  uint32_t init_time_{0};
  bool did_setup_{false};
  uint8_t r_duty_{0x00};
  uint8_t g_duty_{0x00};
  uint8_t b_duty_{0x00};
  uint8_t w_duty_{0x00};
  light::LightState *light_state_{nullptr};

  void _set_channel(float channel_state, uint8_t &duty, ChannelAddr channel_addr);
};
}  // namespace ti_lp5562
}  // namespace esphome
