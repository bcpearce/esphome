#pragma once

#include "esphome/core/component.h"
#include "esphome/components/rgbw/rgbw_light_output.h"
#include "esphome/components/i2c_device/i2c_device.h"

namespace esphome {
namespace ti_lp5562 {
class TiLP5562 : public rgbw::RGBWLightOutput, public i2c_device::I2CDeviceComponent {
 public:
  void setup() override;
  void loop() override;
  void write_state(light::LightState *state) override;

 private:
  uint32_t init_time_{0};
  bool did_setup_{false};
  uint8_t r_pwm_{0x00};
  uint8_t g_pwm_{0x00};
  uint8_t b_pwm_{0x00};
  uint8_t w_pwm_{0x00};
};
}  // namespace ti_lp5562
}  // namespace esphome
