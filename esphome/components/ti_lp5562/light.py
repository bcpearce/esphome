import esphome.codegen as cg
from esphome.components import i2c, light
from esphome.components.rgbw.light import RGBWLightOutput
import esphome.config_validation as cv
from esphome.const import CONF_OUTPUT_ID

DEPENDENCIES = ["i2c"]

ti_lp5562_ns = cg.esphome_ns.namespace("ti_lp5562")
TiLp5562Light = ti_lp5562_ns.class_("TiLp5562Light", RGBWLightOutput, i2c.I2CDevice)

CONFIG_SCHEMA = i2c.i2c_device_schema(0x030).extend(
    {cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(TiLp5562Light)}
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await light.register_light(var, config)
    await i2c.register_i2c_device(var, config)
