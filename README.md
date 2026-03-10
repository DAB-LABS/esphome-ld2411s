# esphome-ld2411s

An ESPHome external component for the **HLK-LD2411S** 24GHz mmWave presence sensor by Hi-Link.

The LD2411S provides independent **motion** and **presence** detection with configurable zones and distance reporting via UART. Unlike simpler radar sensors, it distinguishes between active movement and stationary occupancy — making it well suited for detecting someone sitting still at a desk or workbench.

---

## Features

- Binary **presence** sensor (occupancy detection, including stationary targets)
- Binary **motion** sensor (active movement detection)
- **Distance** sensor (target distance in cm)
- Configurable motion and presence detection zones (min/max in cm)
- Configurable unoccupied timeout
- Apply settings at runtime from Home Assistant — no reflash required
- Radar reboot and factory reset buttons
- Tested on ESP32-WROOM-32 with ESPHome 2026.2.4 and ESP-IDF framework

---

## Hardware

### Sensor
**HLK-LD2411S** — 24GHz mmWave radar module, UART interface, 256000 baud  
Available from Hi-Link and common electronics suppliers.

### Wiring

| LD2411S Pin | Color (typical) | ESP32 DevKit V1 Pin | Notes |
|---|---|---|---|
| VIN | Red | **VIN (5V)** | Must be 5V — **not** 3.3V |
| GND | Black | GND | |
| TXD | Green | GPIO16 (RX) | |
| RXD | Blue | GPIO17 (TX) | |
| OUT | Yellow | Not connected | Optional binary output — not needed with this component |

> **⚠️ The LD2411S requires 5V on VIN.** Wiring it to the ESP32's 3V3 pin will result in the sensor powering on but producing no UART output.

### Power Supply

A USB power supply rated at **1A (5W) minimum** is required. If you experience instability — frequent reboots, dropped WiFi, or sensors going unavailable — try a **2A (10W)** supply before chasing firmware issues.

The ESP32-WROOM-32 and LD2411S together can reach several hundred milliamps of combined peak draw — enough to cause voltage sag on a marginal supply. This is particularly likely during WiFi TX bursts where the ESP32 alone can spike significantly.

> **Tip:** If you see `Reset Reason: power-on event` repeating in ESPHome logs or HA diagnostics, the power supply is the most likely culprit. The diagnostic sensors in the example YAML expose reset reason directly in Home Assistant.

---

## Installation

Add the `external_components` block to your ESPHome YAML:

```yaml
external_components:
  - source: github://DAB-LABS/esphome-ld2411s
    components: [ld2411s]
```

ESPHome will fetch the component directly from this repository at build time.

---

## Configuration

### Minimal example

```yaml
external_components:
  - source: github://DAB-LABS/esphome-ld2411s
    components: [ld2411s]

uart:
  tx_pin: GPIO17
  rx_pin: GPIO16
  baud_rate: 256000
  parity: NONE
  stop_bits: 1

ld2411s:
  distance:
    name: "Target Distance"
  presence:
    name: "Presence"
  motion:
    name: "Motion"
```

### Full example

See [`example.yaml`](example.yaml) for a complete configuration including runtime zone controls, diagnostic sensors, and radar control buttons.

### Component options

```yaml
ld2411s:
  uart_id: uart_bus       # Required only if multiple UART buses are defined
  distance:
    name: "Target Distance"
  presence:
    name: "Presence"
    filters:
      - delayed_off: 10s  # Recommended: prevents flickering on exit
  motion:
    name: "Motion"
    filters:
      - delayed_off: 5s
```

All three sensors (`distance`, `presence`, `motion`) are optional.

---

## Detection Zones

The LD2411S supports two independently configurable detection zones:

| Zone | Purpose |
|---|---|
| **Motion zone** | Detects active movement |
| **Presence zone** | Detects stationary occupancy |

Zone values are in **centimeters**. Hardware minimum is **30cm**.

### Setting Zones at Runtime

The example YAML exposes `number` entities in Home Assistant for adjusting zones without reflashing. Here's how the workflow operates:

1. **Adjust the sliders** in HA for motion zone min/max, presence zone min/max, and unoccupied timeout.
   - Each slider release commits the value to the HA entity — you'll see this reflected in the HA activity log (e.g., `Motion Zone Max changed to 300`). This updates HA's local state but does **not yet** send a command to the radar.

2. **Press Apply Settings** once you've finished adjusting all values.
   - This triggers the `set_config()` call that sends the full zone configuration to the radar in a single command. Set everything first, then apply once — there's no need to press Apply after each individual slider.

3. **Verify** by observing the Distance, Presence, and Motion entities respond from within the new zone boundaries.

### Known Issue: Radar Lock-Up After Zone Changes

The radar can become unresponsive after repeated zone adjustments — particularly when using the slider quickly or applying settings multiple times in rapid succession. Symptoms include:

- Sensors freeze and display the last known state
- Presence, Motion, and Distance entities go **unavailable** while other ESP sensors (WiFi signal, uptime, etc.) remain online

This appears to be a radar firmware issue — the sensor's internal state gets confused by rapid or successive configuration commands. A full ESPHome restart does **not** resolve it; only the radar itself needs to be rebooted.

**Fix:** Press the **Reboot Radar** button in Home Assistant. Sensors typically recover within a few seconds. Factory reset is not required.

> **Best practice:** Make all zone adjustments, press Apply Settings once, then leave it. Avoid sliding and applying repeatedly in quick succession.

---

## ESPHome & Framework Notes

### ESP-IDF framework recommended

```yaml
esp32:
  variant: esp32
  framework:
    type: esp-idf
```

ESP-IDF offers better FreeRTOS scheduler integration, a more reliable WiFi stack, lower memory overhead, and stricter watchdog behavior that makes crashes easier to identify and debug. These qualities matter for a device running continuous UART parsing alongside an active WiFi connection.

### UART wire length

Keep UART wiring (TX/RX) under ~50cm. At 256000 baud on 3.3V logic, signal integrity degrades on longer runs. Mount the ESP32 close to the sensor.

---

## Crash Diagnostics

The `debug` component exposes reset reason and heap health as HA entities. Useful for diagnosing power supply issues, watchdog resets, and memory problems.

```yaml
debug:
  update_interval: 30s

text_sensor:
  - platform: debug
    reset_reason:
      name: "Reset Reason"
      entity_category: diagnostic

sensor:
  - platform: debug
    free:
      name: "Heap Free"
      entity_category: diagnostic
    block:
      name: "Heap Max Block"
      entity_category: diagnostic
    loop_time:
      name: "Loop Time"
      entity_category: diagnostic
```

### Reset Reason reference

| Value | Meaning |
|---|---|
| `power-on event` | Full power loss — check USB supply or loose connections |
| `brownout` | Voltage sag — insufficient power supply |
| `Task watchdog` | Loop starving FreeRTOS scheduler |
| `Panic` | Firmware crash — check serial output for stack trace |
| `Software reset` | Clean restart (OTA, button, or HA restart entity) |

---

## Tested Environment

| Component | Version |
|---|---|
| ESPHome | 2026.2.4 |
| ESP-IDF | 5.5.2 |
| Home Assistant | 2026.2.3 |
| Board | ESP32-WROOM-32 (rev 3.1, dual-core, 4MB flash) |
| Sensor | HLK-LD2411S |

---

## License

MIT
