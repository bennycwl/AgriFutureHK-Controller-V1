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
    // Initialize Status LED explicitly to ensure it doesn't float HIGH on boot
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);

    for (int i = 0; i < 4; i++) {
        ledcSetup(i, freq, resolution);
        ledcAttachPin(pins[i], i);
        
        // Load the last known raw PWM value (0-255) from NVS on boot
        currentPwmValue[i] = storage->getPwmIndex(i); 
        ledcWrite(i, currentPwmValue[i]);
        
        Serial.printf("Channel %d restored to PWM: %d\n", i + 1, currentPwmValue[i]);
    }
}

// Physical button handler: jumps to the next standard level based on current value
void Controller::cycleChannel(uint8_t channelIndex) {
    if (channelIndex >= 4) return;
    
    uint8_t currentVal = currentPwmValue[channelIndex];
    uint8_t nextVal = 0; // Default to 0 (roll over)
    
    // Find the next predefined level that is strictly greater than the current value
    for (int i = 0; i < 5; i++) {
        if (pwmLevels[i] > currentVal) {
            nextVal = pwmLevels[i];
            break;
        }
    }
    
    setChannelPWM(channelIndex, nextVal);
}

uint8_t Controller::getChannelPwmValue(uint8_t channelIndex) {
    if (channelIndex >= 4) return 0;
    return currentPwmValue[channelIndex];
}

// Universal command for CLI, MQTT, and Web: Accepts exact 0-255 value
void Controller::setChannelPWM(uint8_t channelIndex, uint8_t pwmValue) {
    if (channelIndex >= 4) return;
    
    currentPwmValue[channelIndex] = pwmValue;
    ledcWrite(channelIndex, pwmValue);
    
    // Persist raw value to memory
    storage->setPwmIndex(channelIndex, pwmValue);
    
    // Notify MQTT/Web of the state change
    if (onStateChange) onStateChange(channelIndex, pwmValue);
    
    Serial.printf("Channel %d updated to PWM: %d (Saved to memory)\n", channelIndex + 1, pwmValue);
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
        case MQTT_CONNECT:
            analogWrite(ledPin, 220); 
            break;

        case TRYING_CONNECT: // Blinking: 500ms ON / 500ms OFF
            if (now - lastStatusChange >= 500) {
                step = step  % 2;
                if (step == 0) {
                    analogWrite(LED_PIN, 230); // ON
                } else {
                    analogWrite(LED_PIN, 255); // OFF
                }   
                lastStatusChange = now;
            }
            break;

        case AP_MODE: // 1 Long (1000ms), 1 Short (200ms)
            // Cycle: ON 1000ms -> OFF 200ms -> ON 200ms -> OFF 1000ms
            if (step == 0 && (now - lastStatusChange >= 1000)) { analogWrite(LED_PIN, 230); step = 1; lastStatusChange = now; }
            else if (step == 1 && (now - lastStatusChange >= 200)) { analogWrite(LED_PIN, 255); step = 2; lastStatusChange = now; }
            else if (step == 2 && (now - lastStatusChange >= 200)) { analogWrite(LED_PIN, 230); step = 3; lastStatusChange = now; }
            else if (step == 3 && (now - lastStatusChange >= 1000)) { analogWrite(LED_PIN, 255); step = 0; lastStatusChange = now; }
            break;

        case WIFI_CONNECTED: // 2 Long (1000ms), 1 Short (200ms)
            // Cycle: ON 1000ms -> OFF 200ms -> ON 1000ms -> OFF 200ms -> ON 200ms -> OFF 1000ms
            if (step == 0 && (now - lastStatusChange >= 1000)) { analogWrite(LED_PIN, 230); step = 1; lastStatusChange = now; }
            else if (step == 1 && (now - lastStatusChange >= 200)) { analogWrite(LED_PIN, 255); step = 2; lastStatusChange = now; }
            else if (step == 2 && (now - lastStatusChange >= 1000)) { analogWrite(LED_PIN, 230); step = 3; lastStatusChange = now; }
            else if (step == 3 && (now - lastStatusChange >= 200)) { analogWrite(LED_PIN, 255); step = 4; lastStatusChange = now; }
            else if (step == 4 && (now - lastStatusChange >= 200)) { analogWrite(LED_PIN, 230); step = 5; lastStatusChange = now; }
            else if (step == 5 && (now - lastStatusChange >= 1000)) { analogWrite(LED_PIN, 255); step = 0; lastStatusChange = now; }
            break;
    }
}

void Controller::setStateChangeCallback(StateChangeCallback cb) {
    onStateChange = cb;
}