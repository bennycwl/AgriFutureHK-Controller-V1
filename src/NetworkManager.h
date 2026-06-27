#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <functional>
#include "StorageManager.h"

class NetworkManager {
private:
    StorageManager* storage;
    unsigned long lastCheckTime;
    bool connectedPrinted;
    
    WiFiManager wm;
    
    // Pointers for custom portal parameters
    WiFiManagerParameter* custom_mqtt;
    WiFiManagerParameter* custom_comp;
    WiFiManagerParameter* custom_loc;
    WiFiManagerParameter* custom_rack;
    WiFiManagerParameter* custom_lvl;
    WiFiManagerParameter* custom_dev;

    // Callback when user hits "Save" in the captive portal
    void saveParamsCallback();

public:
    NetworkManager(StorageManager* store);
    ~NetworkManager();
    String getIpAddress();
    String getMacAddress();
    void begin();
    void update();
    void resetWifi();
    bool isConnected();
    void saveCredentials(String ssid, String pass);
};

#endif