#include "cli.h"
#include "config.h"
#include "pwm.h"

void handleCli() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() == 0) return;

    Serial.println("> " + cmd);

    if (cmd.startsWith("SET SSID ")) {
      wifi_ssid = cmd.substring(9);
      saveConfig();
      Serial.println("OK: SSID set.");
    } else if (cmd.startsWith("SET PWD ")) {
      wifi_pass = cmd.substring(8);
      saveConfig();
      Serial.println("OK: Password set.");
    } else if (cmd.startsWith("SET MQTT ")) {
      mqtt_server = cmd.substring(9);
      saveConfig();
      Serial.println("OK: MQTT IP set.");
    } else if (cmd.startsWith("SET CO ")) {
      mqtt_company = cmd.substring(7);
      saveConfig();
      Serial.println("OK.");
    } else if (cmd.startsWith("SET LOC ")) {
      mqtt_loc = cmd.substring(8);
      saveConfig();
      Serial.println("OK.");
    } else if (cmd.startsWith("SET RACK ")) {
      mqtt_rack = cmd.substring(9);
      saveConfig();
      Serial.println("OK.");
    } else if (cmd.startsWith("SET LVL ")) {
      mqtt_lvl = cmd.substring(8);
      saveConfig();
      Serial.println("OK.");
    } else if (cmd.startsWith("SET DEV ")) {
      mqtt_dev = cmd.substring(8);
      saveConfig();
      Serial.println("OK.");
    } else if (cmd.startsWith("SET PWM ")) {
      int ch = cmd.substring(8, 9).toInt() - 1;
      int lvl = cmd.substring(10).toInt();
      if (ch >= 0 && ch < 4 && lvl >= 0 && lvl <= 4) {
        applyPWM(ch, lvl, true);
        Serial.printf("OK: CH%d set to index %d (%d)\n", ch + 1, lvl, PWM_LEVELS[lvl]);
      } else {
        Serial.println("ERR: Invalid format. Use: SET PWM <1-4> <0-4>");
      }
    } else if (cmd == "GET MAC") {
      Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());
    } else if (cmd == "GET STATUS") {
      Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());
      Serial.printf("SSID: %s\n", wifi_ssid.c_str());
      Serial.printf("MQTT: %s\n", mqtt_server.c_str());
      Serial.printf("Full Topic: %s\n", getBaseTopic().c_str());
      for (int i = 0; i < 4; i++) {
        Serial.printf("CH%d: Index %d, Val: %d\n", i + 1, currentPwmIndex[i], PWM_LEVELS[currentPwmIndex[i]]);
      }
    } else if (cmd == "REBOOT") {
      Serial.println("Rebooting...");
      ESP.restart();
    } else {
      Serial.println("ERR: Unknown command.");
    }
  }
}
