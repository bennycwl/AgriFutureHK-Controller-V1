#include <Arduino.h>
#include "StorageManager.h"
#include "Controller.h"
#include "ButtonHandler.h"
#include "CliHandler.h"
#include "NetworkManager.h"

// Hardware Pinouts
const uint8_t PWM_PINS[4] = {1, 0, 3, 2};
const uint8_t BTN_PINS[4] = {6, 7, 5, 10};

StorageManager storageManager;
Controller controller(PWM_PINS[0], PWM_PINS[1], PWM_PINS[2], PWM_PINS[3], &storageManager);
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
}