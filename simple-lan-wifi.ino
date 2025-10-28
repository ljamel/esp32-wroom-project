#include <WiFi.h>

// Crée un réseau LAN et transforme l’ESP32 en point d’accès Wi-Fi.

const char* ssid = "ESP32_WiFi";
const char* password = "12345678"; // minimum 8 caractères

void setup() {
  Serial.begin(115200);

  // Crée un point d'accès WiFi
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(ssid, password);

  Serial.print("Point d'accès actif : ");
  Serial.println(ssid);
  Serial.print("IP locale : ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  // Pas besoin de loop pour juste être WiFi
}
