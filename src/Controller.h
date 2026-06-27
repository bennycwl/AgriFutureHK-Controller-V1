#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>

class LedController {
private:
    uint8_t pins[4];
    uint8_t currentLevelIdx[4];
    const uint8_t pwmLevels[5] = {0, 64, 128, 192, 255};
    const int freq = 5000;
    const int resolution = 8; // 8-bit resolution (0-255)

public:
    LedController(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4);
    void begin();
    void cycleChannel(uint8_t channelIndex);
    uint8_t getChannelPwmValue(uint8_t channelIndex);
    
    // We will use this later in Phase 2 for loading from NVS memory
    void setChannelIndex(uint8_t channelIndex, uint8_t levelIdx); 
};

#endif