# ESP32-C3 SuperMini AgriFutureHK Controller

## Features
- 4 digital outputs (PWM) for LED/fan control.
- 4 digital inputs (buttons) to cycle PWM levels (0, 64, 128, 192, 255).
- Level retention after power cycle (Preferences).
- Web UI for configuration and manual control (`http://<hostname>.local`).
- MQTT control and stat reporting with dynamic topic structure.
- Initial WiFi onboarding via AP mode.
- CLI-friendly via serial commands for configuration.

## Setup with PlatformIO
1. `pio run -t upload`
2. On first boot, connect to WiFi AP `AgriFutureHK` (password `12345678`).
3. Open browser to `http://192.168.4.1` and configure WiFi and MQTT.
4. After reboot, visit `http://<hostname>.local` to control outputs.

## CLI
Over serial, type:
- `help` for commands
- `set wifi <ssid> <password>` 
- `set mqtt <broker> <port>`
- `set topic <owner> <project> <room> <location> <device>`
- `status` to show current WiFi/MQTT and channel levels
