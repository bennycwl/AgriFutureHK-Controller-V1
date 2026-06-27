#include "NetworkManager.h"

NetworkManager::NetworkManager(StorageManager* store) {
    storage = store;
    lastCheckTime = 0;
    connectedPrinted = false;
}

void NetworkManager::begin() {
    String ssid = storage->getWifiSsid();
    String pass = storage->getWifiPass();

    // Force the ESP32 to act only as a client (no Access Point)
    WiFi.mode(WIFI_STA); 

    if (ssid.length() > 0) {
        Serial.printf("Attempting to connect to Wi-Fi: %s\n", ssid.c_str());
        WiFi.begin(ssid.c_str(), pass.c_str());
    } else {
        Serial.println("No Wi-Fi credentials found.");
        Serial.println("-> Use CLI command: set wifi <ssid> <password>");
    }
}

void NetworkManager::update() {
    // Check connection status every 3 seconds without blocking the loop
    if (millis() - lastCheckTime > 3000) {
        lastCheckTime = millis();
        
        if (WiFi.status() == WL_CONNECTED && !connectedPrinted) {
            Serial.println("\n--- Wi-Fi Connected! ---");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            connectedPrinted = true;
        } 
        else if (WiFi.status() != WL_CONNECTED && connectedPrinted) {
            Serial.println("\n--- Wi-Fi Disconnected. Attempting to reconnect... ---");
            WiFi.reconnect();
            connectedPrinted = false;
        }
    }
}

void NetworkManager::resetWifi() {
    WiFi.disconnect(true, true);
    storage->clearWifi();
    Serial.println("Wi-Fi credentials completely wiped.");
}

String NetworkManager::getIpAddress() {
    if (WiFi.status() == WL_CONNECTED) return WiFi.localIP().toString();
    return "Not Connected";
}

String NetworkManager::getMacAddress() {
    return WiFi.macAddress();
}

bool NetworkManager::isConnected() {
    return (WiFi.status() == WL_CONNECTED);
}