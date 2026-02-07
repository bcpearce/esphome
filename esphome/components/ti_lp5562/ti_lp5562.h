#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ti_lp5562 {
class TiLP5562LightOutput : public light::LightOutput, public i2c::I2CDevice, public Component {
 public:
  void set_color_mode(light::ColorMode mode) { mode_ = mode; };
  void setup() override;
  void loop() override;
  void write_state(light::LightState *state) override;
  light::LightTraits get_traits() override;
  float get_setup_priority() const override { return esphome::setup_priority::DATA; }
  void dump_config();

 protected:
  light::ColorMode mode_{light::ColorMode::UNKNOWN};
  uint32_t init_time_{0};
  bool did_setup_{false};
  uint8_t r_pwm_{0x00};
  uint8_t g_pwm_{0x00};
  uint8_t b_pwm_{0x00};
  uint8_t w_pwm_{0x00};
  output::FloatOutput *red_{nullptr};
  output::FloatOutput *green_{nullptr};
  output::FloatOutput *blue_{nullptr};
  output::FloatOutput *white_{nullptr};
};
}  // namespace ti_lp5562
}  // namespace esphome
