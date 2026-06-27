# AgriFutureHK Food Server Rack LED Controller (ESP32-C3)

## Overview
This repository contains the firmware for the AgriFutureHK Food Server Rack LED Controller. Designed to fit within the broader **DOMIX** centralized home automation ecosystem, this module acts as a foundational node for a distributed DCEA (Distributed Controlled Environment Agriculture) architecture. 

Built on the ESP32-C3 SuperMini using PlatformIO, the controller manages 4 independent PWM channels for LED grow lights. It features physical button overrides with memory retention, a captive portal for network configuration, a Serial CLI for deep debugging, and remote telemetry/control via MQTT.

## Features
* **4-Channel PWM Control:** Cycles through 5 precise intensity levels (0, 64, 128, 192, 255) via hardware buttons or remote MQTT commands.
* **State Persistence:** Automatically saves and restores PWM levels and device configurations across power cycles using Non-Volatile Storage (NVS).
* **Wi-Fi Manager:** Utilizes a captive portal to easily configure Wi-Fi and MQTT credentials without hardcoding, allowing rapid deployment.
* **Serial CLI:** Comprehensive command-line interface for local device configuration and status monitoring.
* **Granular MQTT Telemetry:** Individual plain-text topics for each channel, allowing precise integration into the AgriFutureHK DCEA taxonomy.

## Hardware Pinout
| Component | ESP32-C3 GPIO | Type | Notes |
| :--- | :--- | :--- | :--- |
| **PWM Channel 1** | GPIO 1 | Output | LED Driver 1 |
| **PWM Channel 2** | GPIO 0 | Output | LED Driver 2 |
| **PWM Channel 3** | GPIO 3 | Output | LED Driver 3 |
| **PWM Channel 4** | GPIO 2 | Output | LED Driver 4 |
| **Button 1 (Ch 1)** | GPIO 5 | Input | Requires `INPUT_PULLUP` |
| **Button 2 (Ch 2)** | GPIO 6 | Input | Requires `INPUT_PULLUP` |
| **Button 3 (Ch 3)** | GPIO 7 | Input | Requires `INPUT_PULLUP` |
| **Button 4 (Ch 4)** | GPIO 10 | Input | Requires `INPUT_PULLUP` |

## MQTT Topic Structure
The device follows a granular, plain-text MQTT structure. Each channel operates independently with its own status and control topics.

**Base Taxonomy:** `v1/<company>/<location>/<rack>/<level>/<device>/`
*(Default: `v1/agrifuturehk/swh/r001/lv01/dv01/`)*

### Channel 1 Topics
* **Status (Publish):** `.../dv01/ch1/status` (Payload: `0`, `64`, `128`, `192`, or `255`)
* **Control (Subscribe):** `.../dv01/ch1/ctl` (Payload: `0`, `64`, `128`, `192`, or `255`)

### Channel 2 Topics
* **Status (Publish):** `.../dv01/ch2/status` (Payload: `0`, `64`, `128`, `192`, or `255`)
* **Control (Subscribe):** `.../dv01/ch2/ctl` (Payload: `0`, `64`, `128`, `192`, or `255`)

*(Channels 3 and 4 follow the exact same structure).*

## Serial CLI Commands
Connect via Serial Monitor (Baud Rate: 115200) to interact with the device locally.

| Command | Description |
| :--- | :--- |
| `set wifi <ssid> <pass>` | Updates Wi-Fi credentials |
| `set mqtt <ip/url>` | Updates MQTT broker address |
| `set info <comp> <loc> <rack> <lvl> <dev>`| Updates device taxonomy |
| `set pwm <ch> <value>` | Manages local PWM override (e.g., `set pwm 1 128`) |
| `status` | Prints all configurations and current PWM states |
| `reset wifi` | Clears stored Wi-Fi and restarts into Captive Portal |

## Software Dependencies
This project is built in PlatformIO. The `platformio.ini` should include:
* `tzapu/WiFiManager` (For Captive Portal Network Setup)
* `knolleary/PubSubClient` (For lightweight MQTT communication)
* `thomasfredericks/Bounce2` (For reliable button debouncing)

## Building and Flashing
1. Clone the repository and open the folder in **VSCode + PlatformIO**.
2. Connect the ESP32-C3 SuperMini via USB-C.
3. Click **Build** (`✓`) in the PlatformIO toolbar to download dependencies and compile.
4. Click **Upload** (`→`) to flash the firmware.
5. Open the **Serial Monitor** (`🔌`) to view boot logs and use the CLI.