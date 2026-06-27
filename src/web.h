#pragma once

#include <Arduino.h>

extern const char index_html[];

void handleRoot();
void handleSave();
void handleApiStatus();
void handleApiSetPwm();
void handleApiIdentify();
