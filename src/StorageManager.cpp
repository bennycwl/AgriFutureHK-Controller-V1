#include "StorageManager.h"

void StorageManager::begin() {
    // Open a namespace called "domix". False means read/write mode.
    preferences.begin("domix", false); 
}

String StorageManager::getMqttBroker() {
    return preferences.getString("mqtt", "test.mosquitto.org");
}
void StorageManager::setMqttBroker(String broker) {
    preferences.putString("mqtt", broker);
}

String StorageManager::getCompany() {
    return preferences.getString("comp", "agrifuturehk");
}
void StorageManager::setCompany(String company) {
    preferences.putString("comp", company);
}

String StorageManager::getLocation() {
    return preferences.getString("loc", "swh");
}
void StorageManager::setLocation(String location) {
    preferences.putString("loc", location);
}

String StorageManager::getRack() {
    return preferences.getString("rack", "r001");
}
void StorageManager::setRack(String rack) {
    preferences.putString("rack", rack);
}

String StorageManager::getLevel() {
    return preferences.getString("lvl", "lv01");
}
void StorageManager::setLevel(String level) {
    preferences.putString("lvl", level);
}

String StorageManager::getDevice() {
    return preferences.getString("dev", "dv01");
}
void StorageManager::setDevice(String device) {
    preferences.putString("dev", device);
}

uint8_t StorageManager::getPwmIndex(uint8_t channel) {
    // Generate a unique key for each channel (e.g., "ch0", "ch1")
    char key[4];
    sprintf(key, "ch%d", channel);
    return preferences.getUChar(key, 0); // Default to index 0 (0 PWM)
}

void StorageManager::setPwmIndex(uint8_t channel, uint8_t index) {
    char key[4];
    sprintf(key, "ch%d", channel);
    preferences.putUChar(key, index);
}

String StorageManager::getWifiSsid() {
    return preferences.getString("ssid", "");
}
void StorageManager::setWifiSsid(String ssid) {
    preferences.putString("ssid", ssid);
}
String StorageManager::getWifiPass() {
    return preferences.getString("pass", "");
}
void StorageManager::setWifiPass(String pass) {
    preferences.putString("pass", pass);
}
void StorageManager::clearWifi() {
    preferences.remove("ssid");
    preferences.remove("pass");
}

