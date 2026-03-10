#pragma once
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace ld2411s {

class LD2411SComponent : public Component, public uart::UARTDevice {
 public:
  void set_distance_sensor(sensor::Sensor *s) { distance_sensor_ = s; }
  void set_presence_sensor(binary_sensor::BinarySensor *s) { presence_sensor_ = s; }
  void set_motion_sensor(binary_sensor::BinarySensor *s) { motion_sensor_ = s; }

  void setup() override {}

  void loop() override {
    while (available()) {
      bytes_.push_back(read());
      size_t n = bytes_.size();
      if (n < 2) continue;

      // Data frame ends with 0x55 0x55
      bool end_data = (bytes_[n - 2] == 0x55 && bytes_[n - 1] == 0x55);
      // Command response ends with 0x04 0x03 0x02 0x01
      bool end_cmd = (n >= 4 &&
                      bytes_[n - 4] == 0x04 && bytes_[n - 3] == 0x03 &&
                      bytes_[n - 2] == 0x02 && bytes_[n - 1] == 0x01);

      if (end_data || end_cmd) {
        process_packet_();
        bytes_.clear();
      }

      // Safety: discard if buffer grows too large without a valid frame
      if (bytes_.size() > 64) bytes_.clear();
    }
  }

 protected:
  void process_packet_() {
    if (bytes_.size() < 3) return;

    // 0xAA 0xAA 0x00 — no target
    if (bytes_[0] == 0xAA && bytes_[1] == 0xAA && bytes_[2] == 0x00) {
      if (presence_sensor_) presence_sensor_->publish_state(false);
      if (motion_sensor_)   motion_sensor_->publish_state(false);
      return;
    }

    // 0xAA 0xAA 0x01 — moving target (presence + motion)
    if (bytes_[0] == 0xAA && bytes_[1] == 0xAA && bytes_[2] == 0x01 && bytes_.size() >= 5) {
      if (presence_sensor_) presence_sensor_->publish_state(true);
      if (motion_sensor_)   motion_sensor_->publish_state(true);
      uint16_t dist = (static_cast<uint16_t>(bytes_[4]) << 8) | bytes_[3];
      if (distance_sensor_) distance_sensor_->publish_state(dist);
      return;
    }

    // 0xAA 0xAA 0x02 — stationary target (presence only)
    if (bytes_[0] == 0xAA && bytes_[1] == 0xAA && bytes_[2] == 0x02 && bytes_.size() >= 5) {
      if (presence_sensor_) presence_sensor_->publish_state(true);
      if (motion_sensor_)   motion_sensor_->publish_state(false);
      uint16_t dist = (static_cast<uint16_t>(bytes_[4]) << 8) | bytes_[3];
      if (distance_sensor_) distance_sensor_->publish_state(dist);
      return;
    }
  }

  sensor::Sensor *distance_sensor_{nullptr};
  binary_sensor::BinarySensor *presence_sensor_{nullptr};
  binary_sensor::BinarySensor *motion_sensor_{nullptr};

  std::vector<uint8_t> bytes_;
};

}  // namespace ld2411s
}  // namespace esphome
