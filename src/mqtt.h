#pragma once

void mqttCallback(char* topic, byte* payload, unsigned int length);
void mqttLoop();
