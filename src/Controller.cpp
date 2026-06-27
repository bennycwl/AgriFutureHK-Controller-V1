#include "Controller.h"

Controller::Controller(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, uint8_t pled, StorageManager* store) {
    pins[0] = p1;
    pins[1] = p2;
    pins[2] = p3;
    pins[3] = p4;
    ledPin = pled;
    storage = store;
}

void Controller::begin() {
    for (int i = 0; i < 4; i++) {
        ledcSetup(i, freq, resolution);
        ledcAttachPin(pins[i], i);
        pinMode(ledPin, OUTPUT);
        
        // PHASE 2: Load the last known state from NVS on boot
        currentLevelIdx[i] = storage->getPwmIndex(i);
        ledcWrite(i, pwmLevels[currentLevelIdx[i]]);
        
        Serial.printf("Channel %d restored to PWM: %d\n", i + 1, pwmLevels[currentLevelIdx[i]]);
    }
}

void Controller::cycleChannel(uint8_t channelIndex) {
    if (channelIndex >= 4) return;
    
    currentLevelIdx[channelIndex]++;
    
    if (currentLevelIdx[channelIndex] >= 5) {
        currentLevelIdx[channelIndex] = 0;
    }
    
    uint8_t pwmValue = pwmLevels[currentLevelIdx[channelIndex]];
    ledcWrite(channelIndex, pwmValue);
    
    // PHASE 2: Save the new state directly to NVS
    storage->setPwmIndex(channelIndex, currentLevelIdx[channelIndex]);
    
    Serial.printf("Channel %d cycled to PWM: %d (Saved to memory)\n", channelIndex + 1, pwmValue);
}

uint8_t Controller::getChannelPwmValue(uint8_t channelIndex) {
    if (channelIndex >= 4) return 0;
    return pwmLevels[currentLevelIdx[channelIndex]];
}

void Controller::setChannelIndex(uint8_t channelIndex, uint8_t levelIdx) {
     if (channelIndex >= 4 || levelIdx >= 5) return;
     currentLevelIdx[channelIndex] = levelIdx;
     ledcWrite(channelIndex, pwmLevels[levelIdx]);
     
     // Ensure overriding via MQTT/CLI also saves to memory
     storage->setPwmIndex(channelIndex, levelIdx);
}

void Controller::setStatusMode(StatusMode mode) {
    if (currentStatusMode != mode) {
        currentStatusMode = mode;
        lastStatusChange = 0; // Reset timer
        step = 0;             // Reset pattern step
    }
}

void Controller::updateStatusIndicator() {
    unsigned long now = millis();
    const int LED_PIN = ledPin; // Use the assigned LED pin

    switch (currentStatusMode) {
        case MQTT_CONNECTED:
            digitalWrite(LED_PIN, HIGH); // Solid ON
            break;

        case TRYING_CONNECT: // Blinking: 500ms ON / 500ms OFF
            if (now - lastStatusChange >= 500) {
                digitalWrite(LED_PIN, !digitalRead(LED_PIN));
                lastStatusChange = now;
            }
            break;

        case AP_MODE: // 1 Long (1000ms), 1 Short (200ms)
            // Cycle: ON 1000ms -> OFF 200ms -> ON 200ms -> OFF 1000ms
            if (step == 0 && (now - lastStatusChange >= 1000)) { digitalWrite(LED_PIN, LOW); step = 1; lastStatusChange = now; }
            else if (step == 1 && (now - lastStatusChange >= 200)) { digitalWrite(LED_PIN, HIGH); step = 2; lastStatusChange = now; }
            else if (step == 2 && (now - lastStatusChange >= 200)) { digitalWrite(LED_PIN, LOW); step = 3; lastStatusChange = now; }
            else if (step == 3 && (now - lastStatusChange >= 1000)) { digitalWrite(LED_PIN, HIGH); step = 0; lastStatusChange = now; }
            break;

        case WIFI_CONNECTED: // 2 Long (1000ms), 1 Short (200ms)
            // Cycle: ON 1000ms -> OFF 200ms -> ON 1000ms -> OFF 200ms -> ON 200ms -> OFF 1000ms
            if (step == 0 && (now - lastStatusChange >= 1000)) { digitalWrite(LED_PIN, LOW); step = 1; lastStatusChange = now; }
            else if (step == 1 && (now - lastStatusChange >= 200)) { digitalWrite(LED_PIN, HIGH); step = 2; lastStatusChange = now; }
            else if (step == 2 && (now - lastStatusChange >= 1000)) { digitalWrite(LED_PIN, LOW); step = 3; lastStatusChange = now; }
            else if (step == 3 && (now - lastStatusChange >= 200)) { digitalWrite(LED_PIN, HIGH); step = 4; lastStatusChange = now; }
            else if (step == 4 && (now - lastStatusChange >= 200)) { digitalWrite(LED_PIN, LOW); step = 5; lastStatusChange = now; }
            else if (step == 5 && (now - lastStatusChange >= 1000)) { digitalWrite(LED_PIN, HIGH); step = 0; lastStatusChange = now; }
            break;
    }
}