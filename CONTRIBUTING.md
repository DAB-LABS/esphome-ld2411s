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
└── ld2411s.h       # C++ driver — UART parsing, entity updates, set_config()
```

The component uses a byte-cap pattern in `loop()` to yield to the FreeRTOS scheduler under ESP-IDF. Any changes to `loop()` should preserve this pattern.

## Questions

Open a [Discussion](https://github.com/DAB-LABS/esphome-ld2411s/discussions) rather than an issue for general questions.
