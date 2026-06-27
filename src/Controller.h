#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include <functional>
#include "StorageManager.h"

enum StatusMode {
    MQTT_CONNECT,
    TRYING_CONNECT,
    AP_MODE,
    WIFI_CONNECTED
};

class Controller {
private:
    uint8_t pins[4];
    uint8_t currentPwmValue[4]; // Changed: Track raw 0-255 value instead of index
    const uint8_t pwmLevels[5] = {0, 64, 128, 192, 255}; // Kept for physical button cycling
    const int freq = 5000;
    const int resolution = 8;
    uint8_t ledPin;
    
    StorageManager* storage;
    StatusMode currentStatusMode;
    unsigned long lastStatusChange;
    int step;

public:
    typedef std::function<void(uint8_t, uint8_t)> StateChangeCallback;

    Controller(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, uint8_t pled, StorageManager* store);
    
    void begin();
    void cycleChannel(uint8_t channelIndex);
    uint8_t getChannelPwmValue(uint8_t channelIndex);
    
    // Changed: New method to accept any 0-255 value
    void setChannelPWM(uint8_t channelIndex, uint8_t pwmValue); 
    
    void setStatusMode(StatusMode mode);
    void updateStatusIndicator();
    
private:
    StateChangeCallback onStateChange = nullptr;

public:
    void setStateChangeCallback(StateChangeCallback cb);
};

#endif