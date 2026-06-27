#include "mqtt.h"
#include "config.h"
#include "pwm.h"

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);
  String payloadStr = "";
  for (unsigned int i = 0; i < length; i++) payloadStr += (char)payload[i];

  for (int i = 0; i < 4; i++) {
    String ctlTopic = getBaseTopic() + "/ch0" + String(i + 1) + "/ctl";
    if (topicStr == ctlTopic) {
      int val = payloadStr.toInt();
      int newIndex = 0;
      if (val >= 255) newIndex = 4;
      else if (val >= 192) newIndex = 3;
      else if (val >= 128) newIndex = 2;
      else if (val >= 64) newIndex = 1;
      else newIndex = 0;
      applyPWM(i, newIndex, true);
      break;
    }
  }
}

void mqttLoop() {
  unsigned long currentMillis = millis();
  if (!apMode && mqtt_server != "") {
    if (!mqttClient.connected()) {
      if (currentMillis - lastMqttReconnect > 5000) {
        lastMqttReconnect = currentMillis;
        Serial.println("Attempting MQTT connection...");
        String clientId = "AgriFuture-" + String(WiFi.macAddress());
        if (mqttClient.connect(clientId.c_str())) {
          Serial.println("MQTT connected!");
          for (int i = 0; i < 4; i++) {
            String topic = getBaseTopic() + "/ch0" + String(i + 1) + "/ctl";
            mqttClient.subscribe(topic.c_str());
          }
          for (int i = 0; i < 4; i++) publishStatus(i);
        }
      }
    } else {
      mqttClient.loop();
    }
  }
}
