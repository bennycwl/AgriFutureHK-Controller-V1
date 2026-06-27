#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include "StorageManager.h"

enum StatusMode {
    MQTT_CONNECTED,    // No blink (Solid)
    TRYING_CONNECT,    // Blinking
    AP_MODE,           // 1 long, 1 short
    WIFI_CONNECTED     // 2 long, 1 short
};

class Controller {

private:
    uint8_t pins[4];
    uint8_t currentLevelIdx[4];
    uint8_t ledPin;
    const uint8_t pwmLevels[5] = {0, 64, 128, 192, 255};
    const int freq = 5000;
    const int resolution = 8;
    StorageManager* storage; // Pointer to our memory manager
    StatusMode currentStatusMode;
    unsigned long lastStatusChange;
    int step; // Used to track pattern progress

public:
    // Constructor now requires the storage manager
    Controller(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4,uint8_t pled, StorageManager* store);
    
    void begin();
    void cycleChannel(uint8_t channelIndex);
    uint8_t getChannelPwmValue(uint8_t channelIndex);
    void setChannelIndex(uint8_t channelIndex, uint8_t levelIdx);
    void setStatusMode(StatusMode mode);
    void updateStatusIndicator(); 
};

#endif