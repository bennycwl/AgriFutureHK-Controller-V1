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
        buttons[i].interval(25); 
        buttons[i].setPressedState(LOW); 
    }
}

void ButtonHandler::update() {
    for (int i = 0; i < 4; i++) {
        buttons[i].update();
        if (buttons[i].pressed()) {
            controller->cycleChannel(i);
        }
    }
}