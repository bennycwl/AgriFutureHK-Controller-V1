#include "pwm.h"
#include "config.h"

void applyPWM(int ch, int index, bool publish) {
  if (index < 0) index = 0;
  if (index > 4) index = 4;

  currentPwmIndex[ch] = index;
  int pwmValue = PWM_LEVELS[index];

  analogWrite(PWM_PINS[ch], pwmValue);

  preferences.begin("led_config", false);
  preferences.putInt((String("pwm") + String(ch)).c_str(), index);
  preferences.end();

  if (publish && mqttClient.connected()) {
    publishStatus(ch);
  }
}

void publishStatus(int ch) {
  String topic = getBaseTopic() + "/ch0" + String(ch + 1) + "/stat";
  String payload = String(PWM_LEVELS[currentPwmIndex[ch]]);
  mqttClient.publish(topic.c_str(), payload.c_str());
  Serial.printf("Published: %s -> %s\n", topic.c_str(), payload.c_str());
}

void checkButtons() {
  for (int i = 0; i < 4; i++) {
    int reading = digitalRead(BTN_PINS[i]);
    if (reading != lastBtnState[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading == LOW && lastBtnState[i] == HIGH) {
        int nextIndex = currentPwmIndex[i] + 1;
        if (nextIndex > 4) nextIndex = 0;
        Serial.printf("Button %d pressed. New level: %d\n", i + 1, PWM_LEVELS[nextIndex]);
        applyPWM(i, nextIndex, true);
        while (digitalRead(BTN_PINS[i]) == LOW) { delay(10); }
      }
    }
    lastBtnState[i] = reading;
  }
}
