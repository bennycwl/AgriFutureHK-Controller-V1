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
* [x] Integrate `tzapu/WiFiManager` library for Captive Portal fallback.
* [x] Configure custom parameters in the captive portal (MQTT Broker, Taxonomy) alongside standard SSID/Password.

## Phase 5: MQTT Integration & Telemetry
**Objective:** Connect to the broader DCEA ecosystem.
* [x] Integrate `PubSubClient` library.
* [x] Construct plain-text topics based on stored taxonomy (e.g., `v1/agrifuturehk/swh/r001/lv01/dv01/ch1/...`).
* [x] Implement `callback` function to parse incoming plain-text payloads (`0`, `64`, `128`, `192`, `255`) on the `.../ctl` topics.
* [x] Implement publishing logic: publish to `.../status` topics whenever a hardware button is pressed or a remote command is executed.
* [x] Handle non-blocking MQTT reconnection logic in the `loop()`.

## Phase 6: System Integration & Testing
**Objective:** Ensure the module is stable for deployment in the food server rack.
* [x] Perform long-run stability testing.
* [x] Test power-cycle recovery (Wi-Fi reconnection, PWM state restoration).
* [x] Validate CLI overrides against MQTT overrides to ensure states remain synchronized.

## Phase 7: Autonomous Scheduling, Environmental Simulation & Reliability
**Objective:** Allow the controller to operate time-based rules autonomously while simulating natural photoperiods safely.
* [ ] **NTP & Internal RTC Synchronization:** Utilize `<time.h>` and `sntp.h` to fetch time via `pool.ntp.org` upon successful Wi-Fi connection.
* [ ] **NTP & Internal RTC Synchronization:** Rely on the ESP32-C3's internal RTC to maintain time across temporary network drops. *(Note: Internal RTC resets on hard power cycles, requiring Wi-Fi restoration to resume schedules).*
* [ ] **System Uptime Heartbeat (MQTT):** Replace the static "alive" heartbeat with a JSON payload reporting system uptime (e.g., `{"uptime": "12d 04h 30m 15s"}`). Publish this every 60 seconds.
* [ ] **System Uptime Heartbeat (CLI):** Update the `printStatus()` command to display the formatted system uptime alongside network details.
* [ ] **Smooth LED Fading (Sunrise/Sunset):** Implement non-blocking, time-based interpolation in `Controller.cpp`. Instead of instantly changing PWM (which causes power supply stress), transition the `currentPwm` to `targetPwm` gradually based on a configured `fade_minutes` duration.
* [ ] **Smooth LED Fading (Sunrise/Sunset):** Ensure instant overrides via CLI or hardware buttons immediately cancel any active fade sequences.
* [ ] **Global MQTT Scheduler:** Subscribe to a unified schedule topic: `.../device/schedule/set`.
* [ ] **Global MQTT Scheduler:** Parse incoming JSON payloads containing `on` time, `off` time, `max_pwm`, and `fade_minutes` for each channel.
* [ ] **Global MQTT Scheduler:** Save the schedule to NVS (`StorageManager`) so it survives hardware reboots. Continuously evaluate the schedule against the internal RTC in the main loop to trigger Sunrise/Sunset events dynamically.
* [ ] **Hardware Watchdog Timer (WDT):** Enable the ESP32 Task Watchdog Timer (TWDT) with a 5 to 8-second timeout. Feed the watchdog continuously in the main `loop()`. If the network stack or control logic freezes, the WDT will forcefully reboot the controller, protecting the crop from prolonged system lockups.

## Phase 8: Advanced Networking & Remote Management
**Objective:** Strengthen network reliability and allow complete remote configuration without physical access to the cabinet.
* [ ] **Fixed IP Configuration:** Update `StorageManager` to store static IP properties: IP address, Gateway, Subnet, and Primary DNS.
* [ ] **Fixed IP Configuration:** Update `NetworkManager.cpp` to apply `WiFi.config()` prior to connection if NVS values exist; otherwise, default to DHCP.
* [ ] **Fixed IP Configuration:** Add CLI commands for easy configuration: `set ip <IP> <Gateway> <Subnet>`.
* [ ] **Web-based Remote Management (UI):** Integrate `ESPAsyncWebServer` to serve a lightweight HTML/JS interface without blocking the RTOS. Implement features including live PWM sliders, network configuration fields, and schedule input forms.
* [ ] **Web-based Remote Management (UI):** Match the web route to the exact NVS MQTT taxonomy (e.g., HTTP GET/POST on `/v1/agrifuturehk/swh/r001/lv01/dv01`), maintaining architectural harmony between API and Web interfaces.
* [ ] **Over-The-Air (OTA) Firmware Updates:** Implement OTA capabilities to allow remote flashing of compiled `.bin` files over Wi-Fi. Integrate this into the `ESPAsyncWebServer` as a dedicated `/update` route. *(Critical requirement for managing controllers sealed inside dedicated electrical cabinets).*

---

## Architecture & Data Models

### MQTT Topic Architecture (Example)
* **Base Taxonomy:** `v1/agrifuturehk/swh/r001/lv01/dv01`
* **Channel Control:** `.../ch1/ctl` (Payload: `0-255`)
* **State Report:** `.../ch1/status` (Payload: `0-255`)
* **Uptime Heartbeat:** `.../status` (Payload: `<value in seconds>`)
* **Schedule Configuration:** `.../schedule/set`
* **Schedule Report:** `.../schedule/report`

### Schedule JSON Payload Structure
```json
{
  "mode": "auto",
  "ch1": {
    "on": "06:00",
    "off": "18:00",
    "max_pwm": 255,
    "fade_minutes": 60
  },
  "ch2": {
    "on": "08:00",
    "off": "16:00",
    "max_pwm": 180,
    "fade_minutes": 30
  }
}