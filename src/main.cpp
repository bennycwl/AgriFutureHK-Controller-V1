#include <Arduino.h>
#include "StorageManager.h"
#include "Controller.h"
#include "ButtonHandler.h"
#include "CliHandler.h"
#include "NetworkManager.h"


// Hardware Pinouts
const uint8_t PWM_PINS[4] = {1, 0, 3, 2};
const uint8_t BTN_PINS[4] = {7, 6, 5, 10};
const uint8_t LED_PINS = 8;

StorageManager storageManager;
Controller controller(PWM_PINS[0], PWM_PINS[1], PWM_PINS[2], PWM_PINS[3],LED_PINS, &storageManager);
ButtonHandler buttonHandler(BTN_PINS[0], BTN_PINS[1], BTN_PINS[2], BTN_PINS[3], &controller);
NetworkManager network(&storageManager); 
CliHandler terminal(&storageManager, &controller, &network);

void setup() {
    Serial.begin(115200);
    delay(2000); 
    
    Serial.println("\n--- AgriFutureHK Controller ---");
    
    storageManager.begin(); 
    controller.begin();
    buttonHandler.begin();
    terminal.begin(); 
    network.begin();

    Serial.println("Phase 4 Init Complete. System Ready.");
}

void loop() {
    buttonHandler.update();
    terminal.update(); 
    network.update(); // NEW: Keep the captive portal running in the background

    // 2. Update Status Indicator Logic
    if (!network.isConnected()) {
        controller.setStatusMode(TRYING_CONNECT);
    } else if (true) {
        controller.setStatusMode(MQTT_CONNECTED);
    }
    
    controller.updateStatusIndicator();

  }