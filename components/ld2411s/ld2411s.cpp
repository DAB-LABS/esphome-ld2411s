#include "ld2411s.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ld2411s {

static const char *const TAG = "ld2411s";

// Data frame: AA AA <type> <dist lo> <dist hi> 55 55
static const size_t DATA_FRAME_LEN = 7;
static const size_t SHORT_FRAME_LEN = 5;  // AA AA 00 55 55
static const uint8_t TYPE_NO_TARGET = 0x00;
static const uint8_t TYPE_MOVING = 0x01;
static const uint8_t TYPE_STATIONARY = 0x02;

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
      ESP_LOGW(TAG, "Discarding %d bytes -- no valid frame found", (int) bytes_.size());
      bytes_.clear();
    }
  }
}

void LD2411SComponent::process_packet_() {
  const size_t n = bytes_.size();
  if (n < SHORT_FRAME_LEN)
    return;

  // Read the frame from its tail, so stray bytes ahead of the header
  // (a partial frame after boot or a UART glitch) do not cost a reading.
  uint8_t type;
  uint16_t dist = 0;
  const uint8_t *f = (n >= DATA_FRAME_LEN) ? &bytes_[n - DATA_FRAME_LEN] : nullptr;
  const uint8_t *s = &bytes_[n - SHORT_FRAME_LEN];
  if (f != nullptr && f[0] == 0xAA && f[1] == 0xAA) {
    type = f[2];
    dist = (static_cast<uint16_t>(f[4]) << 8) | f[3];
  } else if (s[0] == 0xAA && s[1] == 0xAA && s[2] == TYPE_NO_TARGET) {
    // Accept a no-target frame without distance bytes as well.
    type = TYPE_NO_TARGET;
  } else {
    return;
  }

  switch (type) {
    case TYPE_NO_TARGET:
      if (presence_sensor_)
        presence_sensor_->publish_state(false);
      if (motion_sensor_)
        motion_sensor_->publish_state(false);
      // No target means no distance; 0 replaces the last reading
      // instead of leaving it frozen.
      publish_distance_(0);
      break;

    case TYPE_MOVING:
      if (presence_sensor_)
        presence_sensor_->publish_state(true);
      if (motion_sensor_)
        motion_sensor_->publish_state(true);
      publish_distance_(dist);
      break;

    case TYPE_STATIONARY:
      if (presence_sensor_)
        presence_sensor_->publish_state(true);
      if (motion_sensor_)
        motion_sensor_->publish_state(false);
      publish_distance_(dist);
      break;

    default:
      ESP_LOGV(TAG, "Unknown frame type 0x%02X", type);
      break;
  }
}

void LD2411SComponent::publish_distance_(uint16_t cm) {
  if (distance_sensor_ == nullptr)
    return;
  // The radar reports about 5 frames per second; only send a reading
  // to Home Assistant when it actually changes.
  if (static_cast<int32_t>(cm) == last_distance_)
    return;
  last_distance_ = cm;
  distance_sensor_->publish_state(cm);
}

}  // namespace ld2411s
}  // namespace esphome
