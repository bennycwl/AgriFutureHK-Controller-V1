#include "web.h"
#include "config.h"
#include "pwm.h"

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1">
<title>AgriFuture LED Dashboard</title>
<style>
  body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 20px; background-color: #e9ecef;}
  h2 { color: #343a40; border-bottom: 2px solid #dee2e6; padding-bottom: 5px; }
  .card { background: white; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); max-width: 450px; margin: 0 auto 20px auto;}
  input[type=text], input[type=password] { width: 100%; padding: 8px; margin: 6px 0 15px 0; border: 1px solid #ced4da; border-radius: 5px; box-sizing: border-box;}
  button { background-color: #28a745; color: white; padding: 12px 15px; border: none; border-radius: 5px; cursor: pointer; width: 100%; font-size: 16px; font-weight: bold;}
  button:hover { background-color: #218838; }
  .btn-identify { background-color: #17a2b8; margin-bottom: 15px; }
  .btn-identify:hover { background-color: #138496; }
  .channel-row { display: flex; align-items: center; justify-content: space-between; margin: 15px 0; padding: 10px; background: #f8f9fa; border-radius: 8px;}
  .channel-row label { font-weight: bold; width: 100px; color: #495057; }
  .val-badge { display: inline-block; background: #007bff; color: white; padding: 3px 8px; border-radius: 12px; font-size: 14px; min-width: 30px; text-align: center; margin-left: 5px;}
  input[type=range] { flex-grow: 1; margin: 0 10px; cursor: pointer; }
  .grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
  .mac-box { background: #e9ecef; padding: 8px; border-radius: 5px; font-family: monospace; text-align: center; margin-bottom: 15px; border: 1px solid #ced4da;}
</style>
</head><body>

<div class="card">
  <h2>Live LED Control</h2>
  <div id="controls">
    <div class="channel-row">
      <label>Channel 1 <span class="val-badge" id="val0">0</span></label>
      <input type="range" id="ch0" min="0" max="4" step="1" oninput="updateVal(0, this.value)" onchange="setPWM(0, this.value)">
    </div>
    <div class="channel-row">
      <label>Channel 2 <span class="val-badge" id="val1">0</span></label>
      <input type="range" id="ch1" min="0" max="4" step="1" oninput="updateVal(1, this.value)" onchange="setPWM(1, this.value)">
    </div>
    <div class="channel-row">
      <label>Channel 3 <span class="val-badge" id="val2">0</span></label>
      <input type="range" id="ch2" min="0" max="4" step="1" oninput="updateVal(2, this.value)" onchange="setPWM(2, this.value)">
    </div>
    <div class="channel-row">
      <label>Channel 4 <span class="val-badge" id="val3">0</span></label>
      <input type="range" id="ch3" min="0" max="4" step="1" oninput="updateVal(3, this.value)" onchange="setPWM(3, this.value)">
    </div>
  </div>
</div>

<div class="card">
  <h2>Network & Topic Config</h2>
  
  <div class="mac-box">MAC: <strong>%MAC%</strong></div>
  <button type="button" class="btn-identify" onclick="identifyDevice()">💡 Identify Device (Blink LED)</button>

  <form action="/save" method="POST">
    <label>WiFi SSID:</label><input type="text" name="ssid" value="%SSID%">
    <label>WiFi Password:</label><input type="password" name="pass" value="%PASS%">
    <label>MQTT Broker IP:</label><input type="text" name="mqtt" value="%MQTT%">
    
    <h3 style="margin-top: 20px; font-size: 16px; color: #6c757d;">MQTT Topic Structure</h3>
    <label>Company:</label><input type="text" name="co" value="%CO%">
    <div class="grid-2">
      <div><label>Location:</label><input type="text" name="loc" value="%LOC%"></div>
      <div><label>Rack:</label><input type="text" name="rack" value="%RACK%"></div>
      <div><label>Level:</label><input type="text" name="lvl" value="%LVL%"></div>
      <div><label>Device:</label><input type="text" name="dev" value="%DEV%"></div>
    </div>
    
    <button type="submit" style="margin-top:15px;">Save & Reboot</button>
  </form>
</div>

<script>
  const levels = [0, 64, 128, 192, 255];
  function updateVal(ch, idx) { document.getElementById('val'+ch).innerText = levels[idx]; }
  function setPWM(ch, idx) { fetch(`/api/set_pwm?ch=${ch}&lvl=${idx}`); }
  function identifyDevice() { 
    fetch('/api/identify'); 
    alert("Identifying! Check the physical device for a rapidly flashing blue light.");
  }
  function fetchStatus() {
    fetch('/api/status').then(r=>r.json()).then(data => {
      for(let i=0; i<4; i++) {
        let slider = document.getElementById('ch'+i);
        if(document.activeElement !== slider) {
          slider.value = data.pwm[i];
          updateVal(i, data.pwm[i]);
        }
      }
    }).catch(e => console.log("Fetch error"));
  }
  setInterval(fetchStatus, 2000);
  window.onload = fetchStatus;
</script>
</body></html>
)rawliteral";

void handleRoot() {
  String html = index_html;
  html.replace("%SSID%", wifi_ssid);
  html.replace("%PASS%", wifi_pass);
  html.replace("%MQTT%", mqtt_server);
  html.replace("%CO%", mqtt_company);
  html.replace("%LOC%", mqtt_loc);
  html.replace("%RACK%", mqtt_rack);
  html.replace("%LVL%", mqtt_lvl);
  html.replace("%DEV%", mqtt_dev);
  html.replace("%MAC%", WiFi.macAddress());
  server.send(200, "text/html", html);
}

void handleSave() {
  if (server.hasArg("ssid")) wifi_ssid = server.arg("ssid");
  if (server.hasArg("pass")) wifi_pass = server.arg("pass");
  if (server.hasArg("mqtt")) mqtt_server = server.arg("mqtt");

  if (server.hasArg("co")) mqtt_company = server.arg("co");
  if (server.hasArg("loc")) mqtt_loc = server.arg("loc");
  if (server.hasArg("rack")) mqtt_rack = server.arg("rack");
  if (server.hasArg("lvl")) mqtt_lvl = server.arg("lvl");
  if (server.hasArg("dev")) mqtt_dev = server.arg("dev");

  saveConfig();
  server.send(200, "text/html", "<h2>Saved successfully! Rebooting device...</h2>");
  delay(1000);
  ESP.restart();
}

void handleApiStatus() {
  String json = "{\"pwm\":[" +
                String(currentPwmIndex[0]) + "," +
                String(currentPwmIndex[1]) + "," +
                String(currentPwmIndex[2]) + "," +
                String(currentPwmIndex[3]) + "]}";
  server.send(200, "application/json", json);
}

void handleApiSetPwm() {
  if (server.hasArg("ch") && server.hasArg("lvl")) {
    int ch = server.arg("ch").toInt();
    int lvl = server.arg("lvl").toInt();

    if (ch >= 0 && ch < 4 && lvl >= 0 && lvl <= 4) {
      applyPWM(ch, lvl, true);
      server.send(200, "text/plain", "OK");
      return;
    }
  }
  server.send(400, "text/plain", "Bad Request");
}

void handleApiIdentify() {
  identifyMode = true;
  identifyEndTime = millis() + 5000;
  Serial.println("Identify mode triggered via Web!");
  server.send(200, "text/plain", "Identifying");
}
