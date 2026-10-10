# Changelog

All notable changes to this project will be documented here.  
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).  
This project uses [Semantic Versioning](https://semver.org/).

---

## [1.1.1] -- 2026-10-10

The v1.1.0 tag was cut before the Bluetooth fix landed, so anyone pinned to `@v1.1.0` got the release notes without the fix. This release tags what has been on `main` since March. No component behavior changes.

### Fixed
- **Radar Bluetooth switch could not turn Bluetooth off** -- the setting was only held in RAM and lost on reboot. `example.yaml` now sends `End config` (which commits to the radar's flash) before re-entering config mode and rebooting.
- **Bluetooth switch showed ON at first boot while the radar was OFF** -- `restore_mode` is now `RESTORE_DEFAULT_OFF`, matching the LD2411S factory default.
- **Unoccupied Timeout allowed values the protocol cannot hold** -- `example.yaml` max is now 6553 s (was 65535). The `send_config` script multiplies by 10 for the protocol's 100 ms units.
- `example.yaml` now declares `board: esp32dev`, which current ESPHome requires.

### Changed
- `example.yaml` secret renamed from `api_key` to `api_encryption_key`. Rename it in your `secrets.yaml` if you copied the example.
- Component C++ split into `ld2411s.h` and `ld2411s.cpp`, with `dump_config()` and a warning when unframed bytes are discarded. Same parsing logic.
- README installation examples pin a release tag.
- Corrected the v1.0.0 power supply note below to match the README (1A minimum, 2A if unstable).

### Added
- `llms.txt`, PR template, issue template config, `FUNDING.yml`, `.gitignore`, `.gitattributes`, README header image and configuration screenshot.

---

## [1.1.0] -- 2026-03-12

### Added
- **Radar Bluetooth switch** -- toggle onboard BLE on/off from Home Assistant without the HLKRadarTool app. Uses protocol command `0x00A4`. (Turning it off only works from v1.1.1.)
- Documented that the OUT pin is hardwired to GND in LD2411S firmware and cannot be used as a presence output

### Fixed
- **`example.yaml` buttons broken** -- Apply Settings, Reboot Radar, and Factory Reset buttons referenced nonexistent C++ methods (`ld2411s_component.set_config()`, `.reboot()`, `.factory_reset()`) that were never implemented in the component. Replaced with raw `uart.write` commands and a `script` block matching the working implementation. Fixes [#1](https://github.com/DAB-LABS/esphome-ld2411s/issues/1).

### Changed
- `example.yaml` now includes the Radar Bluetooth switch
- README updated with full Bluetooth section, OUT pin clarification, and unoccupied timeout / delayed_off explanation

---

## [1.0.0] -- 2026-03-11

### Added
- Initial release of ESPHome external component for the HLK-LD2411S
- Binary presence sensor with configurable `delayed_off`
- Binary motion sensor with configurable `delayed_off`
- Target distance sensor (cm)
- Runtime zone configuration via HA number entities (motion min/max, presence min/max, unoccupied timeout)
- Apply Settings button -- commits all zone config to radar in a single UART transaction
- Radar Reboot button -- reboots LD2411S module without restarting ESP32
- Factory Reset button -- restores LD2411S to factory defaults
- Crash diagnostics: Reset Reason, Heap Free, Heap Max Block, Loop Time
- Power supply guidance (1A minimum, 2A if unstable; Apple 10W confirmed working)
- ESP-IDF framework documentation and rationale
- Tested on ESP32-WROOM-32 with ESPHome 2026.2.4 and ESP-IDF 5.5.2
