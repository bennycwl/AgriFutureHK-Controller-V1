#include "CliHandler.h"

CliHandler::CliHandler(StorageManager* store, Controller* ctrl, NetworkManager* net) {
    storage = store;
    controller = ctrl;
    network = net; // Add this member variable
}

void CliHandler::begin() {
    Serial.println("CLI Initialized. Type 'status' to view settings.");
    
}

// A non-blocking read that collects characters until Enter (\n or \r) is pressed.
void CliHandler::update() {
    while (Serial.available()) {
        char c = Serial.read();
        
        if (c == '\n' || c == '\r') {
            if (inputBuffer.length() > 0) {
                processCommand(inputBuffer);
                inputBuffer = ""; // Clear buffer after execution
            }
        } else {
            inputBuffer += c;
        }
    }
}

// Helper to split string by spaces
String CliHandler::getToken(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void CliHandler::processCommand(String cmd) {
    cmd.trim();
    
    String action = getToken(cmd, ' ', 0);
    String target = getToken(cmd, ' ', 1);

    if (action.equalsIgnoreCase("status")) {
        printStatus();
    } 
    else if (action.equalsIgnoreCase("help")) {
        Serial.println("Available Commands:");
        Serial.println("  status");
        Serial.println("  set wifi <SSID> <Password>");
        Serial.println("  set mqtt <Broker>");
        Serial.println("  set info Company");
        Serial.println("  set info Location");
        Serial.println("  set info Rack");
        Serial.println("  set info Level");
        Serial.println("  set info Device");
        Serial.println("  set pwm <Channel(1-4)> <Value(0,64,128,192,255)>");
        Serial.println("  reset wifi");
        Serial.println("  reboot");
    }

    else if (action.equalsIgnoreCase("set")) {
        if (target.equalsIgnoreCase("wifi")) {
            String ssid = getToken(cmd, ' ', 2);
            String pass = getToken(cmd, ' ', 3);
            
            if (ssid.length() > 0) {
                storage->setWifiSsid(ssid);
                storage->setWifiPass(pass); // Password can be empty for open networks
                Serial.printf("-> Wi-Fi credentials saved: %s\n", ssid.c_str());
                Serial.println("-> Restarting device to apply connection...");
                delay(1000);
                ESP.restart(); // The cleanest way to ensure the Wi-Fi stack boots fresh
            } else {
                Serial.println("-> Error: SSID cannot be empty.");
            }
        } 
        else if (target.equalsIgnoreCase("mqtt")) {
            String broker = getToken(cmd, ' ', 2);
            storage->setMqttBroker(broker);
            Serial.printf("-> MQTT broker updated: %s\n", broker.c_str());
        } 
        else if (target.equalsIgnoreCase("pwm")) {
            int ch = getToken(cmd, ' ', 2).toInt();
            int val = getToken(cmd, ' ', 3).toInt();
            
            // Map the value (0, 64, 128, 192, 255) to the array index (0, 1, 2, 3, 4)
            int idx = -1;
            if (val == 0) idx = 0;
            else if (val == 64) idx = 1;
            else if (val == 128) idx = 2;
            else if (val == 192) idx = 3;
            else if (val == 255) idx = 4;
            
            if (idx != -1 && ch >= 1 && ch <= 4) {
                controller->setChannelIndex(ch - 1, idx); // Map channel 1-4 to index 0-3
                Serial.printf("-> Channel %d overridden to PWM %d via CLI.\n", ch, val);
            } else {
                Serial.println("-> Error: Invalid channel (1-4) or PWM value (0, 64, 128, 192, 255).");
            }
        }

        // --- INDIVIDUAL TAXONOMY COMMANDS ---
        else if (action.equalsIgnoreCase("info")) {
            if (target.equalsIgnoreCase("company")) {
                String val = getToken(cmd, ' ', 2);
                if (val.length() > 0) { storage->setCompany(val); Serial.printf("-> Company updated: %s\n", val.c_str()); }
                else { Serial.println("-> Error: Missing value."); }
            }
            else if (target.equalsIgnoreCase("location")) {
                String val = getToken(cmd, ' ', 2);
                if (val.length() > 0) { storage->setLocation(val); Serial.printf("-> Location updated: %s\n", val.c_str()); }
                else { Serial.println("-> Error: Missing value."); }
            }
            else if (target.equalsIgnoreCase("rack")) {
                String val = getToken(cmd, ' ', 2);
                if (val.length() > 0) { storage->setRack(val); Serial.printf("-> Rack updated: %s\n", val.c_str()); }
                else { Serial.println("-> Error: Missing value."); }
            }
            else if (target.equalsIgnoreCase("level")) {
                String val = getToken(cmd, ' ', 2);
                if (val.length() > 0) { storage->setLevel(val); Serial.printf("-> Level updated: %s\n", val.c_str()); }
                else { Serial.println("-> Error: Missing value."); }
            }
            else if (target.equalsIgnoreCase("device")) {
                String val = getToken(cmd, ' ', 2);
                if (val.length() > 0) { storage->setDevice(val); Serial.printf("-> Device updated: %s\n", val.c_str()); }
                else { Serial.println("-> Error: Missing value."); }
            }
        }
    } 
    else if (action.equalsIgnoreCase("reset")) {
        if (target.equalsIgnoreCase("wifi")) {
            // Wipes the native ESP32 Wi-Fi config to force Captive Portal on reboot
            WiFi.disconnect(false, true); 
            storage->clearWifi();
            Serial.println("-> Wi-Fi credentials cleared. Please restart the device.");
        }
    }

    else if (action.equalsIgnoreCase("reboot")) {
        Serial.println("-> Rebooting device...");
        ESP.restart();
    } 
    else {
        Serial.println("-> Unknown command. Type 'help' for a list of commands.");

    }
}

void CliHandler::printStatus() {
    Serial.println("\n--- Current Device Status ---");
    wifi_mode_t mode = WiFi.getMode();
    Serial.print("Network Mode: ");
    if (mode == WIFI_MODE_STA) Serial.println("Station (STA)");
    else if (mode == WIFI_MODE_AP) Serial.println("Access Point (AP)");
    else if (mode == WIFI_MODE_APSTA) Serial.println("Station + Access Point (AP+STA)");
    else Serial.println("Disconnected / Off");
    Serial.printf("Wi-Fi SSID : %s\n", storage->getWifiSsid().c_str());
    Serial.printf("Connected  : %s\n", network->isConnected() ? "Yes" : "No");
    Serial.printf("IP Address : %s\n", network->getIpAddress().c_str());
    Serial.printf("MAC Address: %s\n", network->getMacAddress().c_str());
    Serial.printf("MQTT Broker: %s\n", storage->getMqttBroker().c_str());
    Serial.printf("Taxonomy   : v1/%s/%s/%s/%s/%s\n", 
        storage->getCompany().c_str(), storage->getLocation().c_str(), 
        storage->getRack().c_str(), storage->getLevel().c_str(), storage->getDevice().c_str());
    
    Serial.println("Outputs:");
    for(int i = 0; i < 4; i++) {
        Serial.printf(" - Channel %d PWM: %d\n", i + 1, controller->getChannelPwmValue(i));
    }
    Serial.println("-----------------------------\n");
}

