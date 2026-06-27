#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

class StorageManager {
private:
    Preferences preferences;

public:
    void begin();
    
    // Wi-Fi Configuration (NEW)
    String getWifiSsid();
    void setWifiSsid(String ssid);
    String getWifiPass();
    void setWifiPass(String pass);
    void clearWifi();

    // Taxonomy & MQTT Configuration (Existing)
    String getMqttBroker();
    void setMqttBroker(String broker);
    String getCompany();
    void setCompany(String company);
    String getLocation();
    void setLocation(String location);
    String getRack();
    void setRack(String rack);
    String getLevel();
    void setLevel(String level);
    String getDevice();
    void setDevice(String device);

    // PWM State Management (Existing)
    uint8_t getPwmIndex(uint8_t channel);
    void setPwmIndex(uint8_t channel, uint8_t index);
};

#endif