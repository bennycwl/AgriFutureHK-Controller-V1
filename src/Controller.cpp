#include "Controller.h"

LedController::LedController(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4) {
    pins[0] = p1;
    pins[1] = p2;
    pins[2] = p3;
    pins[3] = p4;
    
    // Initialize all channels to index 0 (PWM 0)
    for(int i = 0; i < 4; i++) {
        currentLevelIdx[i] = 0;
    }
}

void LedController::begin() {
    for (int i = 0; i < 4; i++) {
        ledcSetup(i, freq, resolution); // Use loop index 'i' as the PWM channel (0-3)
        ledcAttachPin(pins[i], i);
        ledcWrite(i, 0); // Ensure LEDs start OFF
    }
}

void LedController::cycleChannel(uint8_t channelIndex) {
    if (channelIndex >= 4) return; // Bounds check
    
    currentLevelIdx[channelIndex]++;
    
    // Wrap back to 0 if we exceed the 5 levels (index 4)
    if (currentLevelIdx[channelIndex] >= 5) {
        currentLevelIdx[channelIndex] = 0;
    }
    
    uint8_t pwmValue = pwmLevels[currentLevelIdx[channelIndex]];
    ledcWrite(channelIndex, pwmValue);
    
    Serial.printf("Channel %d cycled to PWM: %d\n", channelIndex + 1, pwmValue);
}

uint8_t LedController::getChannelPwmValue(uint8_t channelIndex) {
    if (channelIndex >= 4) return 0;
    return pwmLevels[currentLevelIdx[channelIndex]];
}

void LedController::setChannelIndex(uint8_t channelIndex, uint8_t levelIdx) {
     if (channelIndex >= 4 || levelIdx >= 5) return;
     currentLevelIdx[channelIndex] = levelIdx;
     ledcWrite(channelIndex, pwmLevels[levelIdx]);
}