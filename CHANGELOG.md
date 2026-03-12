# Changelog

All notable changes to this project will be documented here.  
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).  
This project uses [Semantic Versioning](https://semver.org/).

---

## [Unreleased] — v1.1.0

### Added
- **Radar Bluetooth switch** — toggle onboard BLE on/off from Home Assistant without the HLKRadarTool app. Sends protocol command `0x00A4` and automatically reboots the radar for the change to take effect. Optimistic state with `RESTORE_DEFAULT_ON` to match factory default.
- Documented that the OUT pin is hardwired to GND in LD2411S firmware and cannot be used as a presence output

### Fixed
- **`example.yaml` buttons broken** — Apply Settings, Reboot Radar, and Factory Reset buttons referenced nonexistent C++ methods (`ld2411s_component.set_config()`, `.reboot()`, `.factory_reset()`) that were never implemented in the component. Replaced with raw `uart.write` commands and a `script` block matching the working implementation. Fixes [#1](https://github.com/DAB-LABS/esphome-ld2411s/issues/1).

### Changed
- `example.yaml` now includes the Radar Bluetooth switch
- README updated with full Bluetooth section, OUT pin clarification, and unoccupied timeout / delayed_off explanation

---

## [1.0.0] — 2026-03-11

### Added
- Initial release of ESPHome external component for the HLK-LD2411S
- Binary presence sensor with configurable `delayed_off`
- Binary motion sensor with configurable `delayed_off`
- Target distance sensor (cm)
- Runtime zone configuration via HA number entities (motion min/max, presence min/max, unoccupied timeout)
- Apply Settings button — commits all zone config to radar in a single UART transaction
- Radar Reboot button — reboots LD2411S module without restarting ESP32
- Factory Reset button — restores LD2411S to factory defaults
- Crash diagnostics: Reset Reason, Heap Free, Heap Max Block, Loop Time
- Full power supply guidance (2A minimum, Apple 10W confirmed working)
- ESP-IDF framework documentation and rationale
- Tested on ESP32-WROOM-32 with ESPHome 2026.2.4 and ESP-IDF 5.5.2
