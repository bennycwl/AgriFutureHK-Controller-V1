#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "StorageManager.h"

class NetworkManager {
private:
    StorageManager* storage;
    unsigned long lastCheckTime;
    bool connectedPrinted;
    
public:
    NetworkManager(StorageManager* store);
    String getIpAddress();
    String getMacAddress();
    void begin();
    void update();
    void resetWifi();
    bool isConnected();
};

#endif