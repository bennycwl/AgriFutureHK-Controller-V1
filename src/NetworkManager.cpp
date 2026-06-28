#include "NetworkManager.h"

NetworkManager::NetworkManager(StorageManager* store) {
    storage = store;
    lastCheckTime = 0;
    connectedPrinted = false;
    
    // Initialize pointers to null
    custom_mqtt = nullptr;
    custom_comp = nullptr;
    custom_loc = nullptr;
    custom_rack = nullptr;
    custom_lvl = nullptr;
    custom_dev = nullptr;
}

NetworkManager::~NetworkManager() {
    // Clean up dynamic memory
    if(custom_mqtt) delete custom_mqtt;
    if(custom_comp) delete custom_comp;
    if(custom_loc) delete custom_loc;
    if(custom_rack) delete custom_rack;
    if(custom_lvl) delete custom_lvl;
    if(custom_dev) delete custom_dev;
}

void NetworkManager::saveParamsCallback() {
    Serial.println("\n[Captive Portal] Saving parameters...");
    
    // Extract values from the web form and save to NVS
    storage->setMqttBroker(String(custom_mqtt->getValue()));
    storage->setCompany(String(custom_comp->getValue()));
    storage->setLocation(String(custom_loc->getValue()));
    storage->setRack(String(custom_rack->getValue()));
    storage->setLevel(String(custom_lvl->getValue()));
    storage->setDevice(String(custom_dev->getValue()));
    
    Serial.println("[Captive Portal] Parameters saved to NVS successfully.");
}

void NetworkManager::begin() {
    // 1. Initialize parameters with current NVS values to pre-fill the portal
    custom_mqtt = new WiFiManagerParameter("mqtt", "MQTT Broker", storage->getMqttBroker().c_str(), 64);
    custom_comp = new WiFiManagerParameter("comp", "Company", storage->getCompany().c_str(), 32);
    custom_loc  = new WiFiManagerParameter("loc", "Location", storage->getLocation().c_str(), 32);
    custom_rack = new WiFiManagerParameter("rack", "Rack", storage->getRack().c_str(), 32);
    custom_lvl  = new WiFiManagerParameter("lvl", "Level", storage->getLevel().c_str(), 32);
    custom_dev  = new WiFiManagerParameter("dev", "Device", storage->getDevice().c_str(), 32);

    // 2. Attach parameters to WiFiManager
    wm.addParameter(custom_mqtt);
    wm.addParameter(custom_comp);
    wm.addParameter(custom_loc);
    wm.addParameter(custom_rack);
    wm.addParameter(custom_lvl);
    wm.addParameter(custom_dev);

    // 3. Bind the save callback
    wm.setSaveParamsCallback(std::bind(&NetworkManager::saveParamsCallback, this));

    // 4. Critical: Set Non-Blocking mode to protect the main loop()
    wm.setConfigPortalBlocking(false);

    // 5. Check if we have CLI-configured credentials
    String ssid = storage->getWifiSsid();
    String pass = storage->getWifiPass();
    
    if (ssid.length() > 0) {
        Serial.printf("Injecting CLI Wi-Fi credentials for: %s\n", ssid.c_str());
        WiFi.mode(WIFI_STA);
        //WiFi.setTxPower(WIFI_POWER_8_5dBm);
        WiFi.begin(ssid.c_str(), pass.c_str());
    }
    
    // Generate AP Name: AgriFutureHK_Ctrl_<mac>
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    String apName = "AgriFutureHK_Ctrl_" + mac.substring(8);
    Serial.printf("Starting Portal with AP Name: %s\n", apName.c_str());

    // 6. Start the auto-connect sequence. If it fails, it spins up "DOMIX_AP"
    wm.autoConnect(apName.c_str());
}

void NetworkManager::update() {
    // Must be called continuously for the non-blocking captive portal to function
    wm.process();

    // Connection status check (every 3 seconds)
    if (millis() - lastCheckTime > 3000) {
        lastCheckTime = millis();
        
        if (WiFi.status() == WL_CONNECTED && !connectedPrinted) {
            Serial.println("\n--- Wi-Fi Connected! ---");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            connectedPrinted = true;
            
            // Sync CLI storage with the network the portal actually connected to
            if (storage->getWifiSsid() != WiFi.SSID()) {
                storage->setWifiSsid(WiFi.SSID());
            }
        } 
        else if (WiFi.status() != WL_CONNECTED && connectedPrinted) {
            Serial.println("\n--- Wi-Fi Disconnected. Waiting for auto-reconnect or portal... ---");
            connectedPrinted = false;
        }
    }
}

void NetworkManager::resetWifi() {
    wm.resetSettings();      // Wipes WiFiManager's internal credentials
    WiFi.disconnect(true, true);
    storage->clearWifi();    // Wipes our custom CLI credentials
    Serial.println("Wi-Fi credentials and portal settings completely wiped.");
}

String NetworkManager::getIpAddress() {
    if (WiFi.status() == WL_CONNECTED) return WiFi.localIP().toString();
    return "Not Connected";
}

String NetworkManager::getMacAddress() {
    return WiFi.macAddress();
}

bool NetworkManager::isConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

void NetworkManager::saveCredentials(String ssid, String pass) {
    // 1. Save to our NVS via StorageManager (for CLI consistency)
    storage->setWifiSsid(ssid);
    storage->setWifiPass(pass);
    
    // 2. Clear any previous/stale credentials
    WiFi.disconnect(true);
    
    // 3. Connect manually so the system registers the connection
    WiFi.begin(ssid.c_str(), pass.c_str());
    
    Serial.println("[Network] Credentials saved to NVS and connection initialized.");
}