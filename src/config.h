#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <PubSubClient.h>

extern const int PWM_PINS[4];
extern const int BTN_PINS[4];
extern const int STATUS_LED;
extern const int PWM_LEVELS[5];
extern int currentPwmIndex[4];

extern Preferences preferences;
extern WebServer server;
extern WiFiClient espClient;
extern PubSubClient mqttClient;

extern String wifi_ssid;
extern String wifi_pass;
extern String mqtt_server;

extern String mqtt_company;
extern String mqtt_loc;
extern String mqtt_rack;
extern String mqtt_lvl;
extern String mqtt_dev;

String getBaseTopic();

extern bool apMode;
extern unsigned long lastMqttReconnect;
extern unsigned long lastDebounceTime[4];
extern int lastBtnState[4];
extern const unsigned long debounceDelay;

extern unsigned long lastBlinkTime;
extern bool ledBlinkState;
extern const int blinkInterval;

extern unsigned long lastWifiLedTime;
extern int wifiSeqIndex;
extern const int wifiSeq[4];

extern bool identifyMode;
extern unsigned long identifyEndTime;
extern unsigned long lastIdentifyBlink;
extern bool identifyLedState;
extern const int identifyBlinkInterval;

void saveConfig();
void loadConfig();
