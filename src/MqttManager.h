#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "StorageManager.h"
#include "Controller.h"

class MqttManager {
private:
    StorageManager* storage;
    Controller* controller;
    WiFiClient espClient;
    PubSubClient client;
    
    unsigned long lastReconnectAttempt;
    String baseTopic;
    String clientId;
    String brokerAddress;


    void buildBaseTopic();
    void reconnect();

public:
    MqttManager(StorageManager* store, Controller* ctrl);
    void begin();
    void update();
    void mqttCallback(char* topic, byte* payload, unsigned int length);
    void publishState(uint8_t channelIndex, uint8_t pwmValue);
    bool isConnected();
};

#endif