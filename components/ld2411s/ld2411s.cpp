#include "ld2411s.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ld2411s {

static const char *const TAG = "ld2411s";

void LD2411SComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up LD2411S...");
}

void LD2411SComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "LD2411S:");
  ESP_LOGCONFIG(TAG, "  Distance sensor: %s", this->distance_sensor_ ? "yes" : "no");
  ESP_LOGCONFIG(TAG, "  Presence sensor: %s", this->presence_sensor_ ? "yes" : "no");
  ESP_LOGCONFIG(TAG, "  Motion sensor: %s", this->motion_sensor_ ? "yes" : "no");
}

void LD2411SComponent::loop() {
  while (available()) {
    bytes_.push_back(read());
    const size_t n = bytes_.size();
    if (n < 2)
      continue;

    // Data frame ends with 0x55 0x55
    const bool end_data = (bytes_[n - 2] == 0x55 && bytes_[n - 1] == 0x55);
    // Command response ends with 0x04 0x03 0x02 0x01
    const bool end_cmd = (n >= 4 &&
                          bytes_[n - 4] == 0x04 && bytes_[n - 3] == 0x03 &&
                          bytes_[n - 2] == 0x02 && bytes_[n - 1] == 0x01);

    if (end_data || end_cmd) {
      process_packet_();
      bytes_.clear();
    }

    // Safety: discard if buffer grows too large without a valid frame
    if (bytes_.size() > 64) {
      ESP_LOGW(TAG, "Discarding %d bytes — no valid frame found", (int) bytes_.size());
      bytes_.clear();
    }
  }
}

void LD2411SComponent::process_packet_() {
  if (bytes_.size() < 3)
    return;

  // 0xAA 0xAA 0x00 — no target
  if (bytes_[0] == 0xAA && bytes_[1] == 0xAA && bytes_[2] == 0x00) {
    if (presence_sensor_)
      presence_sensor_->publish_state(false);
    if (motion_sensor_)
      motion_sensor_->publish_state(false);
    return;
  }

  // 0xAA 0xAA 0x01 — moving target (presence + motion)
  if (bytes_[0] == 0xAA && bytes_[1] == 0xAA && bytes_[2] == 0x01 && bytes_.size() >= 5) {
    if (presence_sensor_)
      presence_sensor_->publish_state(true);
    if (motion_sensor_)
      motion_sensor_->publish_state(true);
    uint16_t dist = (static_cast<uint16_t>(bytes_[4]) << 8) | bytes_[3];
    if (distance_sensor_)
      distance_sensor_->publish_state(dist);
    return;
  }

  // 0xAA 0xAA 0x02 — stationary target (presence only)
  if (bytes_[0] == 0xAA && bytes_[1] == 0xAA && bytes_[2] == 0x02 && bytes_.size() >= 5) {
    if (presence_sensor_)
      presence_sensor_->publish_state(true);
    if (motion_sensor_)
      motion_sensor_->publish_state(false);
    uint16_t dist = (static_cast<uint16_t>(bytes_[4]) << 8) | bytes_[3];
    if (distance_sensor_)
      distance_sensor_->publish_state(dist);
    return;
  }
}

}  // namespace ld2411s
}  // namespace esphome
