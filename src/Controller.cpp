#include "Controller.h"

Controller::Controller(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, StorageManager* store) {
    pins[0] = p1;
    pins[1] = p2;
    pins[2] = p3;
    pins[3] = p4;
    storage = store;
}

void Controller::begin() {
    for (int i = 0; i < 4; i++) {
        ledcSetup(i, freq, resolution);
        ledcAttachPin(pins[i], i);
        
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