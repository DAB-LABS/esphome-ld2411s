import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor, binary_sensor
from esphome.const import (
    CONF_ID,
    UNIT_CENTIMETER,
    STATE_CLASS_MEASUREMENT,
)

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "binary_sensor"]

ld2411s_ns = cg.esphome_ns.namespace("ld2411s")
LD2411SComponent = ld2411s_ns.class_(
    "LD2411SComponent", cg.Component, uart.UARTDevice
)

CONF_DISTANCE = "distance"
CONF_PRESENCE = "presence"
CONF_MOTION = "motion"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LD2411SComponent),
            cv.Optional(CONF_DISTANCE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CENTIMETER,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
                icon="mdi:ruler",
            ),
            cv.Optional(CONF_PRESENCE): binary_sensor.binary_sensor_schema(
                device_class="occupancy",
            ),
            cv.Optional(CONF_MOTION): binary_sensor.binary_sensor_schema(
                device_class="motion",
            ),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_DISTANCE in config:
        sens = await sensor.new_sensor(config[CONF_DISTANCE])
        cg.add(var.set_distance_sensor(sens))

    if CONF_PRESENCE in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_PRESENCE])
        cg.add(var.set_presence_sensor(bs))

    if CONF_MOTION in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_MOTION])
        cg.add(var.set_motion_sensor(bs))
