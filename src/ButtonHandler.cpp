#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, Controller* ctrl) {
    pins[0] = p1;
    pins[1] = p2;
    pins[2] = p3;
    pins[3] = p4;
    controller = ctrl;
}

void ButtonHandler::begin() {
    for (int i = 0; i < 4; i++) {
        buttons[i].attach(pins[i], INPUT_PULLUP);
        buttons[i].interval(40); // Standard debounce
        buttons[i].setPressedState(LOW); 
    }
}

void ButtonHandler::update() {
    for (int i = 0; i < 4; i++) {
        buttons[i].update();
        
        if (buttons[i].pressed()) {
            // --- MITIGATION: Strict Software Verification Filter ---
            bool validPress = true;
            
            // Sample the pin 50 times over a 25ms window
            for (int sample = 0; sample < 50; sample++) {
                delayMicroseconds(500); // 0.5ms delay per sample
                
                if (digitalRead(pins[i]) == HIGH) {
                    // The pin bounced back HIGH! This is high-frequency RF/electrical noise.
                    validPress = false;
                    break; 
                }
            }
            
            if (!validPress) {
                // Serial.printf("Discarded RF/Electrical noise on Button %d\n", i + 1);
                continue; // Skip execution, do not cycle the light
            }
            
            // If it survived the continuous LOW check, a human definitely pressed it
            controller->cycleChannel(i);
        }
    }
}