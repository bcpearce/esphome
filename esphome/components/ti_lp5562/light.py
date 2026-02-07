import esphome.codegen as cg
from esphome.components import i2c, light
from esphome.components.light.types import COLOR_MODES
import esphome.config_validation as cv
from esphome.const import CONF_COLOR_MODE, CONF_OUTPUT_ID

DEPENDENCIES = ["i2c"]

ti_lp5562_ns = cg.esphome_ns.namespace("ti_lp5562")
TiLp5562Light = ti_lp5562_ns.class_(
    "TiLP5562LightOutput", light.LightOutput, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    light.RGB_LIGHT_SCHEMA.extend(
        {
            cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(TiLp5562Light),
            cv.Optional(CONF_COLOR_MODE, default="RGB_WHITE"): cv.enum(
                {
                    k: COLOR_MODES[k]
                    for k in ["RGB", "RGB_WHITE", "WHITE", "BRIGHTNESS"]
                },
                upper=True,
                space="_",
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x30))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await cg.register_component(var, config)
    await light.register_light(var, config)
    await i2c.register_i2c_device(var, config)
    cg.add(var.set_color_mode(config[CONF_COLOR_MODE]))
