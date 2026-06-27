#ifndef CLI_HANDLER_H
#define CLI_HANDLER_H

#include <Arduino.h>
#include "StorageManager.h"
#include "Controller.h"
#include <WiFi.h>
#include "NetworkManager.h"

class CliHandler {
private:
    StorageManager* storage;
    Controller* controller;
    String inputBuffer;
    NetworkManager* network;

    void processCommand(String cmd);
    void printStatus();
    String getToken(String data, char separator, int index);

public:
    CliHandler(StorageManager* store, Controller* ctrl, NetworkManager* net);
    void begin();
    void update();
};

#endif