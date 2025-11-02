#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

const char* AP_SSID = "ESP8266_Ultimate";
const char* AP_PASS = "12345678";

ESP8266WebServer server(80);
WebSocketsServer ws(81);

const char page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP8266 Monitor Dashboard</title>
<style>
:root { --bg:#0f172a; --card:#1e293b; --accent:#38bdf8; --txt:#f1f5f9; }
body{margin:0;background:var(--bg);color:var(--txt);display:flex;flex-direction:column;align-items:center;min-height:100vh;overflow-x:hidden;font-family:Inter,Segoe UI,Arial,sans-serif;}
h1{margin:24px 0;font-weight:700;font-size:1.5rem;text-align:center;}
.container{width:90%;max-width:900px;display:grid;grid-template-columns:repeat(auto-fit,minmax(260px,1fr));gap:20px;}
.card{background:var(--card);padding:20px;border-radius:16px;box-shadow:0 4px 20px rgba(0,0,0,.4);transition:transform .2s;}
.card:hover{transform:translateY(-3px);}
.value{font-size:2.2rem;font-weight:700;margin-bottom:8px;}
.label{color:#94a3b8;font-size:.9rem;margin-bottom:4px;}
button{border:0;padding:12px 18px;border-radius:8px;font-size:1rem;cursor:pointer;background:var(--accent);color:#000;font-weight:600;}
#sphere{width:120px;height:120px;border-radius:50%;margin:0 auto;background:radial-gradient(circle at 30% 30%,#38bdf8,#0ea5e9,#0369a1);box-shadow:0 0 30px #38bdf8;animation:spin 5s linear infinite;}
@keyframes spin{from{transform:rotate(0deg);}to{transform:rotate(360deg);}}
footer{margin:30px 0;font-size:.8rem;color:#64748b;}
</style>
</head>
<body>
<h1>🌐 ESP8266 Monitor Dashboard</h1>
<div id="sphere"></div>
<div class="container">
  <div class="card">
    <div class="label">Température</div>
    <div class="value" id="temp">-- °C</div>
    <div class="label">Signal Wi-Fi</div>
    <div class="value" id="rssi">-- dBm</div>
  </div>
  <div class="card">
    <div class="label">Adresse IP</div>
    <div class="value" id="ip">--</div>
    <div class="label">Uptime</div>
    <div class="value" id="uptime">--</div>
  </div>
  <div class="card">
    <div class="label">LED intégrée</div>
    <div class="value" id="ledState">OFF</div>
    <button id="btnLed">Basculer</button>
  </div>
</div>
<footer>ESP8266 — WebSocket temps réel</footer>
<script>
const tempEl=document.getElementById('temp'), rssiEl=document.getElementById('rssi'),
 ipEl=document.getElementById('ip'), uptimeEl=document.getElementById('uptime'),
 ledEl=document.getElementById('ledState');
const ws=new WebSocket('ws://'+location.hostname+':81/');
ws.onmessage=(e)=>{
 try{
  const d=JSON.parse(e.data);
  tempEl.textContent=d.temp.toFixed(1)+' °C';
  rssiEl.textContent=d.rssi+' dBm';
  ipEl.textContent=d.ip;
  uptimeEl.textContent=d.uptime;
  ledEl.textContent=d.led?'ON':'OFF';
 }catch(err){}
};
document.getElementById('btnLed').onclick=()=>ws.send('toggle');
</script>
</body>
</html>
)rawliteral";

void handleRoot() { server.send_P(200, "text/html", page); }

void onWsEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_TEXT) {
    String msg = (char*)payload;
    if (msg == "toggle") {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      bool ledOn = (digitalRead(LED_BUILTIN) == LOW);
      String resp = String("{\"led\":") + (ledOn ? "true" : "false") + "}";
      ws.sendTXT(num, resp);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  Serial.println();
  Serial.println("=== ESP8266 Ultimate Dashboard ===");
  Serial.print("SSID : "); Serial.println(AP_SSID);
  Serial.print("PASS : "); Serial.println(AP_PASS);
  Serial.print("IP : "); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
  ws.begin();
  ws.onEvent(onWsEvent);
}

unsigned long last = 0;

void loop() {
  server.handleClient();
  ws.loop();

  if (millis() - last > 1500) {
    last = millis();

    // 🔹 Simulation température (20°C à 35°C)
    float tempC = 20.0 + (float)(random(0, 150)) / 10.0;

    String j = "{";
    j += "\"temp\":" + String(tempC, 1) + ",";
    j += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    j += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
    j += "\"uptime\":\"" + String(millis()/1000) + " s\",";
    bool ledOn = (digitalRead(LED_BUILTIN) == LOW);
    j += "\"led\":"; j += (ledOn ? "true" : "false");
    j += "}";

    ws.broadcastTXT(j);
  }
}
