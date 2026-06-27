#include "config.h"
#include "web.h"
#include "mqtt.h"
#include "cli.h"
#include "pwm.h"
#include <ESPmDNS.h>

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  Serial.println("\n--- AgriFutureHK LED Controller Booting ---");
  delay(10000);

  for (int i = 0; i < 4; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
    pinMode(PWM_PINS[i], OUTPUT);
  }

  loadConfig();

  for (int i = 0; i < 4; i++) {
    applyPWM(i, currentPwmIndex[i], false);
  }

  if (wifi_ssid != "") {
    wifi_ssid.trim();
    wifi_pass.trim();

    Serial.printf("Connecting to WiFi: '%s'\n", wifi_ssid.c_str());

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.setAutoReconnect(true);
    WiFi.disconnect(true);
    delay(100);

    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
      delay(500);
      Serial.print(".");
      retries++;
    }
    Serial.println();
  }

  if (WiFi.status() != WL_CONNECTED) {
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    String ap_ssid = "AgriFuture_" + mac.substring(mac.length() - 4);

    Serial.printf("Starting AP Mode: '%s'\n", ap_ssid.c_str());
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid.c_str());
    apMode = true;
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
  }

  String mac_mdns = WiFi.macAddress();
  mac_mdns.replace(":", "");
  String unique_id = mac_mdns.substring(mac_mdns.length() - 4);
  String mdns_name = "agrifuture-" + unique_id;
  mdns_name.toLowerCase();

  if (!MDNS.begin(mdns_name.c_str())) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.printf("mDNS started. Reachable at http://%s.local\n", mdns_name.c_str());
    MDNS.addService("http", "tcp", 80);
  }

  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.on("/api/status", handleApiStatus);
  server.on("/api/set_pwm", handleApiSetPwm);
  server.on("/api/identify", handleApiIdentify);
  server.begin();

  if (mqtt_server != "") {
    mqtt_server.trim();
    mqttClient.setServer(mqtt_server.c_str(), 1883);
    mqttClient.setCallback(mqttCallback);
  }
}

void loop() {
  server.handleClient();
  handleCli();
  checkButtons();

  unsigned long currentMillis = millis();

  if (identifyMode) {
    if (currentMillis > identifyEndTime) {
      identifyMode = false;
    } else if (currentMillis - lastIdentifyBlink >= identifyBlinkInterval) {
      lastIdentifyBlink = currentMillis;
      identifyLedState = !identifyLedState;
      digitalWrite(STATUS_LED, identifyLedState);
    }
  } else {
    if (apMode || WiFi.status() != WL_CONNECTED) {
      digitalWrite(STATUS_LED, (wifiSeqIndex % 2 == 0) ? LOW : HIGH);
      if (currentMillis - lastWifiLedTime >= wifiSeq[wifiSeqIndex]) {
        lastWifiLedTime = currentMillis;
        wifiSeqIndex = (wifiSeqIndex + 1) % 4;
      }
    } else if (mqtt_server != "" && !mqttClient.connected()) {
      wifiSeqIndex = 0;
      lastWifiLedTime = currentMillis;
      if (currentMillis - lastBlinkTime >= blinkInterval) {
        lastBlinkTime = currentMillis;
        ledBlinkState = !ledBlinkState;
        digitalWrite(STATUS_LED, ledBlinkState);
      }
    } else {
      wifiSeqIndex = 0;
      lastWifiLedTime = currentMillis;
      digitalWrite(STATUS_LED, HIGH);
    }
  }

  mqttLoop();
}
