#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

WebServer server(80);

// === CONFIG WIFI ACCESS POINT ===
const char* ssid = "ESP32-Monitor";
const char* password = "esp32test";

unsigned long lastUpdate = 0;
float fakeTemp = 30.0;
String logs = "";

// Fonction simulant une température interne (à défaut de capteur)
float getInternalTemp() {
  uint16_t adc = analogRead(34); // lecture bruit analogique
  return 25.0 + ((adc % 1000) / 100.0); // valeur pseudo-aléatoire
}

// Page web principale
String htmlPage() {
  String page = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset="utf-8">
    <title>ESP32 Monitor</title>
    <style>
      body { font-family: Arial; background: #111; color: #eee; text-align: center; }
      h1 { color: #0ff; }
      .data { margin: 10px; padding: 10px; background: #222; border-radius: 10px; display: inline-block; }
      button { padding: 10px 20px; border: none; border-radius: 10px; background: #0ff; color: #000; font-weight: bold; cursor: pointer; }
      canvas { background: #000; border-radius: 10px; }
    </style>
  </head>
  <body>
    <h1>ESP32 Diagnostic Web Server</h1>
    <div class="data">
      <p><b>Uptime:</b> <span id="uptime">0</span> s</p>
      <p><b>Température interne:</b> <span id="temp">--</span> °C</p>
      <p><b>RSSI WiFi:</b> <span id="rssi">--</span> dBm</p>
      <p><b>Fréquence CPU:</b> <span id="cpu">--</span> MHz</p>
    </div>
    <canvas id="chart" width="300" height="100"></canvas>

    <script>
      let ctx = document.getElementById("chart").getContext("2d");
      let temps = [];
      function update() {
        fetch("/data").then(r=>r.json()).then(d=>{
          document.getElementById("uptime").innerText = d.uptime;
          document.getElementById("temp").innerText = d.temp.toFixed(2);
          document.getElementById("rssi").innerText = d.rssi;
          document.getElementById("cpu").innerText = d.cpu;
          temps.push(d.temp);
          if (temps.length > 30) temps.shift();
          ctx.clearRect(0,0,300,100);
          ctx.beginPath();
          ctx.strokeStyle="#0f0";
          for(let i=0;i<temps.length;i++){
            ctx.lineTo(i*10,100-temps[i]*2);
          }
          ctx.stroke();
        });
      }
      setInterval(update, 1000);
      function downloadLogs(){
        window.location="/download";
      }
    </script>
  </body>
  </html>
  )rawliteral";
  return page;
}

// === ROUTES ===
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void handleData() {
  unsigned long uptime = millis() / 1000;
  float temp = getInternalTemp();
  int32_t rssi = WiFi.RSSI();
  int cpuFreq = getCpuFrequencyMhz();
  
  // Ajout aux logs
  logs += String(uptime) + "s | Temp=" + String(temp) + "°C | RSSI=" + String(rssi) + "dBm\n";
  if (logs.length() > 5000) logs.remove(0, 2000); // évite débordement mémoire

  String json = "{\"uptime\":" + String(uptime) + ",\"temp\":" + String(temp,2) +
                ",\"rssi\":" + String(rssi) + ",\"cpu\":" + String(cpuFreq) + "}";
  server.send(200, "application/json", json);
}

void handleDownload() {
  String filename = "/esp32_logs.txt";
  File file = SPIFFS.open(filename, FILE_WRITE);
  if (file) {
    file.print(logs);
    file.close();
  }
  File dl = SPIFFS.open(filename, FILE_READ);
  if (!dl) {
    server.send(500, "text/plain", "Erreur SPIFFS");
    return;
  }
  server.streamFile(dl, "text/plain");
  dl.close();
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  Serial.println("\n[Démarrage ESP32 Monitor]");

  // Démarre SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("Erreur SPIFFS");
    return;
  }

  // Création du point d'accès WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.print("Point d'accès: ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  // Routes
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/download", handleDownload);
  server.begin();
  Serial.println("Serveur web prêt !");
}

void loop() {
  server.handleClient();
}
