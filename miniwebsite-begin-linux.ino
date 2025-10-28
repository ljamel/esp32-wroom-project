#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

// === CONFIGURATION DU POINT D'ACCÈS WIFI ===
const char* ssid = "LinuxGaming-Lab";
const char* password = "linux4fun";

String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<title>🎮 Linux Gaming Lab - ESP32</title>
<style>
  body {
    background-color: #0d1117;
    color: #c9d1d9;
    font-family: Consolas, monospace;
    margin: 0;
    padding: 20px;
  }
  h1 {
    color: #58a6ff;
    text-align: center;
    margin-bottom: 10px;
  }
  .section {
    background: #161b22;
    border: 1px solid #30363d;
    border-radius: 10px;
    padding: 15px;
    margin: 15px 0;
  }
  code {
    background: #21262d;
    color: #79c0ff;
    padding: 3px 6px;
    border-radius: 5px;
  }
  button {
    background: #238636;
    border: none;
    color: white;
    padding: 8px 15px;
    border-radius: 8px;
    cursor: pointer;
    font-weight: bold;
  }
  button:hover {
    background: #2ea043;
  }
  .quiz {
    background: #1f6feb;
    color: white;
    padding: 10px;
    border-radius: 10px;
  }
</style>
</head>
<body>
  <h1>🐧 Linux Gaming Lab</h1>
  <div class="section">
    <h2>🎯 Objectif</h2>
    <p>Apprendre à jouer sous Linux grâce à <b>Proton</b>, <b>Wine</b> et <b>Lutris</b>, directement depuis un mini-serveur ESP32 !</p>
  </div>

  <div class="section">
    <h2>🧩 Bases du Gaming Linux</h2>
    <ul>
      <li><b>Steam + Proton :</b> Lance la majorité des jeux Windows sous Linux via une couche de compatibilité.</li>
      <li><b>Lutris :</b> Gère tes jeux (Steam, GOG, Epic) sur une seule interface.</li>
      <li><b>Wine :</b> Permet d’exécuter les applications Windows directement sur Linux.</li>
      <li><b>DXVK :</b> Traduit DirectX vers Vulkan pour de meilleures performances graphiques.</li>
    </ul>
  </div>

  <div class="section">
    <h2>💻 Commandes Linux essentielles pour le Gaming</h2>
    <pre>
sudo apt update && sudo apt upgrade
sudo apt install steam lutris wine
protontricks --gui
glxinfo | grep "OpenGL renderer"
    </pre>
  </div>

  <div class="section">
    <h2>🧠 Quiz Rapide</h2>
    <p>Quel outil permet de lancer les jeux Windows sur Steam sous Linux ?</p>
    <button onclick="checkAnswer('Wine')">Wine</button>
    <button onclick="checkAnswer('Proton')">Proton</button>
    <button onclick="checkAnswer('Lutris')">Lutris</button>
    <p id="result"></p>
  </div>

  <div class="section quiz">
    <h3>Astuce bonus</h3>
    <p>Utilise <code>gamemoded</code> pour booster automatiquement les performances de ton CPU et GPU pendant les sessions de jeu.</p>
  </div>

  <script>
    function checkAnswer(ans) {
      let res = document.getElementById('result');
      if(ans === 'Proton') {
        res.innerHTML = "✅ Correct ! Proton est l'outil de compatibilité de Steam pour Linux.";
        res.style.color = "#00ff7f";
      } else {
        res.innerHTML = "❌ Incorrect. Essaie encore !";
        res.style.color = "#ff5555";
      }
    }
  </script>
</body>
</html>
)rawliteral";

// ROUTE PRINCIPALE
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println("\n=== Serveur Linux Gaming prêt ===");
  Serial.print("SSID : "); Serial.println(ssid);
  Serial.print("Mot de passe : "); Serial.println(password);
  Serial.print("IP : "); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
