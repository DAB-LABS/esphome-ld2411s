# Changelog

All notable changes to this project will be documented here.  
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).  
This project uses [Semantic Versioning](https://semver.org/).

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
- AHT30 temperature and humidity sensor support in example.yaml
- Crash diagnostics: Reset Reason, Heap Free, Heap Max Block, Loop Time
- Full power supply guidance (2A minimum, Apple 10W confirmed working)
- ESP-IDF framework documentation and rationale
- Tested on ESP32-WROOM-32 with ESPHome 2026.2.4 and ESP-IDF 5.5.2
