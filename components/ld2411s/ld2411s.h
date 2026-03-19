#pragma once
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

#include <vector>

namespace esphome {
namespace ld2411s {

class LD2411SComponent : public Component, public uart::UARTDevice {
 public:
  void set_distance_sensor(sensor::Sensor *s) { distance_sensor_ = s; }
  void set_presence_sensor(binary_sensor::BinarySensor *s) { presence_sensor_ = s; }
  void set_motion_sensor(binary_sensor::BinarySensor *s) { motion_sensor_ = s; }

  void setup() override;
  void loop() override;
  void dump_config() override;

  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void process_packet_();

  sensor::Sensor *distance_sensor_{nullptr};
  binary_sensor::BinarySensor *presence_sensor_{nullptr};
  binary_sensor::BinarySensor *motion_sensor_{nullptr};

  std::vector<uint8_t> bytes_;
};

}  // namespace ld2411s
}  // namespace esphome
