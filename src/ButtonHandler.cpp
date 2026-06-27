#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, LedController* controller) {
    pins[0] = p1;
    pins[1] = p2;
    pins[2] = p3;
    pins[3] = p4;
    ledCtrl = controller;
}

void ButtonHandler::begin() {
    for (int i = 0; i < 4; i++) {
        // Attach pin with internal pullup
        buttons[i].attach(pins[i], INPUT_PULLUP);
        buttons[i].interval(25); // 25ms debounce interval
        
        // Since we use INPUT_PULLUP, the pin reads LOW when pressed
        buttons[i].setPressedState(LOW); 
    }
}

void ButtonHandler::update() {
    for (int i = 0; i < 4; i++) {
        buttons[i].update();
        
        // If the button was just pressed down this frame
        if (buttons[i].pressed()) {
            ledCtrl->cycleChannel(i);
        }
    }
}