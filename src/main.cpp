#include <Arduino.h>
#include "StorageManager.h"
#include "Controller.h"
#include "ButtonHandler.h"
#include "CliHandler.h"
#include "NetworkManager.h"
#include "MqttManager.h" // NEW

const uint8_t PWM_PINS[4] = {1, 0, 3, 2};
const uint8_t BTN_PINS[4] = {7, 6, 5, 10};
const uint8_t LED_PINS = 8;

StorageManager storageManager;
Controller controller(PWM_PINS[0], PWM_PINS[1], PWM_PINS[2], PWM_PINS[3], LED_PINS, &storageManager);
ButtonHandler buttonHandler(BTN_PINS[0], BTN_PINS[1], BTN_PINS[2], BTN_PINS[3], &controller);
NetworkManager network(&storageManager); 
MqttManager mqttManager(&storageManager, &controller); // NEW
CliHandler terminal(&storageManager, &controller, &network, &mqttManager); 

void setup() {
    Serial.begin(115200);
    delay(2000); 
    
    Serial.println("\n--- AgriFutureHK Controller ---");
    
    storageManager.begin(); 
    controller.begin();
    buttonHandler.begin();
    terminal.begin(); 
    network.begin();
    mqttManager.begin(); // NEW

    // NEW: Link hardware state changes to the MQTT publisher.
    // Whenever a button is pressed or CLI command is issued, this pushes to the broker.
    controller.setStateChangeCallback([](uint8_t channelIndex, uint8_t pwmValue) {
        mqttManager.publishState(channelIndex, pwmValue);
    });

    Serial.println("Phase 5 Init Complete. System Ready.");
}

void loop() {
    buttonHandler.update();
    terminal.update(); 
    network.update(); 
    mqttManager.update(); // NEW: Keep MQTT alive and process messages

    static unsigned long lastBlinkTime = 0;
    static bool ledState = false;

    // Update Status Indicator Logic
    if (!network.isConnected()) {
        if (WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA) {
            controller.setStatusMode(AP_MODE);
        } else {
            controller.setStatusMode(TRYING_CONNECT);
        }
    } else {
        // NEW: Check MQTT connection status to finalize the LED indicator logic
        if (mqttManager.isConnected()) {
            controller.setStatusMode(MQTT_CONNECT); // Solid ON
        } else {
            controller.setStatusMode(WIFI_CONNECTED); // 2 Long, 1 Short
        }
    }
    controller.updateStatusIndicator();
}