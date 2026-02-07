import esphome.codegen as cg
from esphome.components import i2c, light
from esphome.components.rgbw import light as rgbw_light
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_OUTPUT_ID

DEPENDENCIES = ["i2c", "rgbw"]

ti_lp5562_ns = cg.esphome_ns.namespace("ti_lp5562")
TiLp5562Light = ti_lp5562_ns.class_("TiLp5562Light", i2c.I2CDevice)

CONFIG_SCHEMA = rgbw_light.CONFIG_SCHEMA.extend(
    {cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(TiLp5562Light)}
).extend(i2c.i2c_device_schema(0x30))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await light.register_light(var, config)
    await i2c.register_i2c_device(var, config)
