# Project Plan: AgriFutureHK LED Controller (ESP32-C3)

## Overview
This document outlines the development phases for the AgriFutureHK Food Server Rack LED Controller. The architecture prioritizes a non-blocking RTOS/Loop structure, modular C++ design, and robust state retention.

---

## Phase 1: Foundation & Hardware Abstraction
**Objective:** Establish the PlatformIO environment and basic hardware I/O.
* [x] Initialize PlatformIO project for `esp32-c3-devkitm-1` (or generic C3).
* [x] Implement `Controller.cpp/h` to manage the ESP32 `ledc` peripheral (GPIO 1, 0, 3, 2).
* [x] Implement `ButtonHandler.cpp/h` using the `Bounce2` library for GPIO 6, 7, 5, 10 (`INPUT_PULLUP`).
* [x] Map button presses to cycle the respective PWM channel through values: 0, 64, 128, 192, 255.

## Phase 2: Memory & State Persistence
**Objective:** Ensure configuration and states survive power loss.
* [x] Implement `StorageManager.cpp/h` using the ESP32 `Preferences` library (NVS).
* [x] Define default taxonomy (`agrifuturehk`, `swh`, `r001`, `lv01`, `dv01`).
* [x] Define default MQTT broker (`test.mosquitto.org`).
* [x] Save PWM state to NVS upon change; load and apply PWM state during `setup()`.

## Phase 3: Command Line Interface (CLI)
**Objective:** Create a local debug and configuration interface.
* [x] Implement a non-blocking Serial reader in the main `loop()`.
* [x] Parse incoming strings for commands (`set wifi`, `set mqtt`, `set info`, `set pwm`, `status`).
* [x] Link CLI actions to `StorageManager` to update NVS immediately.

## Phase 4: Networking (Wi-Fi Integration)
**Objective:** Connect the device to the local network for future telemetry.
* [x] Implement `NetworkManager.cpp/h` to handle basic `WIFI_STA` connections.
* [x] Read SSID and Password from NVS (configured via CLI) to establish a connection on boot.
* [x] Handle non-blocking Wi-Fi reconnection logic in the `loop()`.
* [ ] **Pending:** Integrate `tzapu/WiFiManager` library for Captive Portal fallback.
* [ ] **Pending:** Configure custom parameters in the captive portal (MQTT Broker, Taxonomy) alongside standard SSID/Password.

## Phase 5: MQTT Integration & Telemetry
**Objective:** Connect to the broader DCEA ecosystem.
* [ ] Integrate `PubSubClient` library.
* [ ] Construct plain-text topics based on stored taxonomy (e.g., `v1/agrifuturehk/swh/r001/lv01/dv01/ch1/...`).
* [ ] Implement `callback` function to parse incoming plain-text payloads (`0`, `64`, `128`, `192`, `255`) on the `.../ctl` topics.
* [ ] Implement publishing logic: publish to `.../status` topics whenever a hardware button is pressed or a remote command is executed.
* [ ] Handle non-blocking MQTT reconnection logic in the `loop()`.

## Phase 6: System Integration & Testing
**Objective:** Ensure the module is stable for deployment in the food server rack.
* [ ] Perform long-run stability testing.
* [ ] Test power-cycle recovery (Wi-Fi reconnection, PWM state restoration).
* [ ] Validate CLI overrides against MQTT overrides to ensure states remain synchronized.