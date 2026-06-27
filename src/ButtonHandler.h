#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>
#include <Bounce2.h>
#include "Controller.h"

class ButtonHandler {
private:
    Bounce2::Button buttons[4];
    uint8_t pins[4];
    LedController* ledCtrl;

public:
    ButtonHandler(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4, LedController* controller);
    void begin();
    void update();
};

#endif