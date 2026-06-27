#include "config.h"

const int PWM_PINS[4] = {1, 0, 3, 2};
const int BTN_PINS[4] = {5, 6, 7, 10};
const int STATUS_LED = 8;

const int PWM_LEVELS[5] = {0, 64, 128, 192, 255};
int currentPwmIndex[4] = {0, 0, 0, 0};

Preferences preferences;
WebServer server(80);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

String wifi_ssid = "";
String wifi_pass = "";
String mqtt_server = "test.mosquitto.org";

String mqtt_company = "agrifuturehk";
String mqtt_loc = "swh";
String mqtt_rack = "r001";
String mqtt_lvl = "lv01";
String mqtt_dev = "dv01";

bool apMode = false;
unsigned long lastMqttReconnect = 0;
unsigned long lastDebounceTime[4] = {0, 0, 0, 0};
int lastBtnState[4] = {HIGH, HIGH, HIGH, HIGH};
const unsigned long debounceDelay = 50;

unsigned long lastBlinkTime = 0;
bool ledBlinkState = HIGH;
const int blinkInterval = 500;

unsigned long lastWifiLedTime = 0;
int wifiSeqIndex = 0;
const int wifiSeq[4] = {600, 200, 200, 1000};

bool identifyMode = false;
unsigned long identifyEndTime = 0;
unsigned long lastIdentifyBlink = 0;
bool identifyLedState = HIGH;
const int identifyBlinkInterval = 50;

String getBaseTopic() {
  return "v1/" + mqtt_company + "/" + mqtt_loc + "/" + mqtt_rack + "/" + mqtt_lvl + "/" + mqtt_dev;
}

void loadConfig() {
  preferences.begin("sys_config", true);
  wifi_ssid = preferences.getString("ssid", "");
  wifi_pass = preferences.getString("pass", "");
  mqtt_server = preferences.getString("mqtt", "");

  mqtt_company = preferences.getString("co", "agrifuturehk");
  mqtt_loc = preferences.getString("loc", "swh");
  mqtt_rack = preferences.getString("rack", "r001");
  mqtt_lvl = preferences.getString("lvl", "lv01");
  mqtt_dev = preferences.getString("dev", "dv01");
  preferences.end();

  preferences.begin("led_config", true);
  for (int i = 0; i < 4; i++) {
    currentPwmIndex[i] = preferences.getInt((String("pwm") + String(i)).c_str(), 0);
  }
  preferences.end();
}

void saveConfig() {
  preferences.begin("sys_config", false);
  preferences.putString("ssid", wifi_ssid);
  preferences.putString("pass", wifi_pass);
  preferences.putString("mqtt", mqtt_server);

  preferences.putString("co", mqtt_company);
  preferences.putString("loc", mqtt_loc);
  preferences.putString("rack", mqtt_rack);
  preferences.putString("lvl", mqtt_lvl);
  preferences.putString("dev", mqtt_dev);
  preferences.end();
}
