# Contributing

Thanks for your interest in improving this component. Here's how to get involved.

---

## Reporting Bugs

Use the [Bug Report](.github/ISSUE_TEMPLATE/bug_report.md) issue template.

The most useful things to include:
- ESPHome version
- ESP-IDF version
- Board (ESP32-WROOM-32 or other)
- Reset Reason from the diagnostic sensor
- Heap Free and Loop Time at the time of the issue
- Relevant ESPHome logs (use `logger: level: DEBUG` if needed)

## Suggesting Features

Use the [Feature Request](.github/ISSUE_TEMPLATE/feature_request.md) issue template.  
Check existing issues first to avoid duplicates.

## Submitting a Pull Request

1. Fork the repo and create a branch from `main`
2. Make your changes to the component files under `components/ld2411s/`
3. Test against a real HLK-LD2411S if possible — this component is hardware-dependent
4. Update `example.yaml` if your change adds new YAML options
5. Update `CHANGELOG.md` under an `[Unreleased]` section
6. Open a PR with a clear description of what changed and why

## Component Structure

```
components/ld2411s/
├── __init__.py     # ESPHome component registration and YAML schema
├── ld2411s.h       # C++ header — class declaration, setters
└── ld2411s.cpp     # C++ driver — UART parsing, frame processing, entity updates
```

The component reads UART data in `loop()` and accumulates bytes until a valid frame delimiter is found (data frame: `0x55 0x55`, command response: `0x04 0x03 0x02 0x01`). A 64-byte safety cap discards malformed data to prevent unbounded memory growth. Zone configuration and radar commands are sent via raw `uart.write` sequences in the YAML — the C++ component only handles reading.

## Questions

Open a [Discussion](https://github.com/DAB-LABS/esphome-ld2411s/discussions) rather than an issue for general questions.
