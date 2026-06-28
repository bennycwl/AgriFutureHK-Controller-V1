#include "MqttManager.h"

MqttManager::MqttManager(StorageManager* store, Controller* ctrl) : client(espClient) {
    storage = store;
    controller = ctrl;
    lastReconnectAttempt = 0;
}

void MqttManager::buildBaseTopic() {
    baseTopic = "v1/" + storage->getCompany() + "/" + 
                storage->getLocation() + "/" + 
                storage->getRack() + "/" + 
                storage->getLevel() + "/" + 
                storage->getDevice();
    baseTopic.toLowerCase();
}

void MqttManager::begin() {
    buildBaseTopic();
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    clientId = "AgriFutureHK_Ctrl_" + mac;

    brokerAddress = storage->getMqttBroker(); 
    
    if (brokerAddress.length() > 0) {
        espClient.setTimeout(5);
        client.setServer(brokerAddress.c_str(), 1883);

        // Set the callback, but DO NOT call client.connect() here! 
        // Connecting here blocks the setup() and freezes the LED.
        client.setCallback([this](char* topic, byte* payload, unsigned int length) {
            this->mqttCallback(topic, payload, length);
        });
    }
}

void MqttManager::reconnect() {
    if (millis() - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = millis();
        Serial.print("\n[MQTT] Attempting connection to ");
        Serial.print(storage->getMqttBroker());
        Serial.print("... ");
        
        String lwtTopic = baseTopic + "/status";
        
        // Connect with Last Will and Testament (LWT)
        if (client.connect(clientId.c_str(), nullptr, nullptr, lwtTopic.c_str(), 1, true, "offline")) {
            Serial.println("[MQTT] Connected!");
            
            // Publish online status
            client.publish(lwtTopic.c_str(), "online", true);
            
            // 1. Subscribe to control topics for all 4 channels
            for (int i = 1; i <= 4; i++) {
                String subTopic = baseTopic + "/ch" + String(i) + "/ctl";
                client.subscribe(subTopic.c_str());
            }
            
            // 2. Publish the initial/current state of all channels on connect
            for (int i = 0; i < 4; i++) {
                publishState(i, controller->getChannelPwmValue(i));
            }
        } else {
            Serial.printf("Failed, rc=%d. Trying again in 5s.\n", client.state());
        }
    }
}

void MqttManager::update() {
    static unsigned long lastHeartbeat = 0;
    
    // --- STRICT GUARD: Do absolutely nothing if Wi-Fi is not connected! ---
    if (WiFi.status() != WL_CONNECTED) {
        return; 
    }

    if (!client.connected() && storage->getMqttBroker().length() > 0) {
        reconnect();
    } else if (client.connected()) {
        client.loop(); // Process incoming messages non-stop
        
        // Send heartbeat every 60 seconds
        if (millis() - lastHeartbeat > 60000) {
            lastHeartbeat = millis();
            String heartbeatTopic = baseTopic + "/heartbeat";
            client.publish(heartbeatTopic.c_str(), "alive");
        }
    }
}

void MqttManager::mqttCallback(char* topic, byte* payload, unsigned int length) {
    String msg = "";
    for (unsigned int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }
    
    String t = String(topic);
    int chIndex = -1;
    if (t.endsWith("/ch1/ctl")) chIndex = 0;
    else if (t.endsWith("/ch2/ctl")) chIndex = 1;
    else if (t.endsWith("/ch3/ctl")) chIndex = 2;
    else if (t.endsWith("/ch4/ctl")) chIndex = 3;

    if (chIndex != -1) {
        int val = msg.toInt();
        if (val >= 0 && val <= 255) {
            controller->setChannelPWM(chIndex, (uint8_t)val);
            Serial.printf("[MQTT Rx] Channel %d set to PWM: %d\n", chIndex + 1, val);
        } else {
            Serial.println("[MQTT] Error: Value out of range (0-255).");
        }
    }
}

void MqttManager::publishState(uint8_t channelIndex, uint8_t pwmValue) {
    if (client.connected()) {
        String pubTopic = baseTopic + "/ch" + String(channelIndex + 1) + "/status";
        client.publish(pubTopic.c_str(), String(pwmValue).c_str(), true); 
        Serial.printf("[MQTT Tx] Topic: %s | Payload: %d\n", pubTopic.c_str(), pwmValue);
    }
}

bool MqttManager::isConnected() {
    return client.connected();
}