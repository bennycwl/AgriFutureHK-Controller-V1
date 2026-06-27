#include <Arduino.h>
#include "Controller.h"
#include "ButtonHandler.h"

// Define GPIO layouts
const uint8_t PWM_PINS[4] = {1, 0, 3, 2};
const uint8_t BTN_PINS[4] = {5, 6, 7, 10};

// Instantiate the hardware managers
LedController ledController(PWM_PINS[0], PWM_PINS[1], PWM_PINS[2], PWM_PINS[3]);
ButtonHandler buttonHandler(BTN_PINS[0], BTN_PINS[1], BTN_PINS[2], BTN_PINS[3], &ledController);

void setup() {
    Serial.begin(115200);
    
    // Brief delay to allow the USB CDC Serial to connect before printing
    delay(2000); 
    
    Serial.println("\n--- AgriFutureHK DOMIX ---");
    Serial.println("Initializing LED Controller...");

    ledController.begin();
    buttonHandler.begin();

    Serial.println("Phase 1 Init Complete. System Ready.");
}

void loop() {
    // Non-blocking loop continuously checks for hardware button presses
    buttonHandler.update();
}