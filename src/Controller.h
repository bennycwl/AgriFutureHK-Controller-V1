#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include "StorageManager.h"

class Controller {
private:
    uint8_t pins[4];
    uint8_t currentLevelIdx[4];
    const uint8_t pwmLevels[5] = {0, 64, 128, 192, 255};
    const int freq = 5000;
    const int resolution = 8;
    
    StorageManager* storage; // Pointer to our memory manager

public:
    // Constructor now requires the storage manager
    Controller(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, StorageManager* store);
    
    void begin();
    void cycleChannel(uint8_t channelIndex);
    uint8_t getChannelPwmValue(uint8_t channelIndex);
    void setChannelIndex(uint8_t channelIndex, uint8_t levelIdx); 
};

#endif