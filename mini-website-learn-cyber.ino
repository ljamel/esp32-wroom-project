#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

const char* ssid = "CyberCTF-ESP32";
const char* password = "cyberlab";

String logs = "";

// HTML page terminal-like avec explications
String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<title>ESP32 Cyber Lab</title>
<style>
body { background:#000; color:#0f0; font-family: monospace; padding:10px;}
#terminal { width:100%; height:80vh; background:#000; color:#0f0; border:1px solid #0f0; overflow:auto; padding:10px;}
input { width:90%; background:#111; color:#0f0; border:none; padding:5px; margin-top:5px;}
button { background:#0f0; color:#000; border:none; padding:5px 10px; margin-left:5px; cursor:pointer;}
</style>
</head>
<body>
<h1>ESP32 Cyber Lab - Exercices</h1>
<div id="terminal">
Bienvenue dans le Cyber Lab ESP32 !<br>
Tapez 'help' pour voir la liste des exercices et commandes.<br><br>

<b>Exercices disponibles :</b><br>
1. <b>caesar texte decalage</b> - Chiffrement César.<br>
2. <b>rot13 texte</b> - Chiffrement ROT13.<br>
3. <b>pwdcheck motdepasse</b> - Test mot de passe faible.<br>
4. <b>ipscan</b> - Analyse d'IP/MAC fictives.<br>
5. <b>hashcheck</b> - Génération de hash fictif.<br>
6. <b>quiz</b> - Question CTF.<br>
7. <b>base64 encode texte</b> - Encodage Base64.<br>
8. <b>portscan</b> - Vérifier ports fictifs.<br>
9. <b>bruteforce</b> - Simulation brute-force mot de passe.<br>
10. <b>detect string</b> - Analyse chaîne suspecte.<br>
</div>
<input type="text" id="cmd" placeholder="Tapez ici..." autofocus>
<button onclick="sendCmd()">Exécuter</button>

<script>
function appendTerm(text){
  let term = document.getElementById('terminal');
  term.innerHTML += "&gt; "+text+"<br>";
  term.scrollTop = term.scrollHeight;
}

function sendCmd(){
  let cmd = document.getElementById('cmd').value;
  fetch('/exec?cmd=' + encodeURIComponent(cmd))
    .then(r=>r.text())
    .then(t=>{ appendTerm(cmd); appendTerm(t); });
  document.getElementById('cmd').value='';
}
</script>
</body>
</html>
)rawliteral";

// Exécution commandes
String handleCommand(String cmd){
  cmd.trim();
  String resp = "";

  if(cmd=="help"){
    resp="Commandes: help, caesar, rot13, pwdcheck, ipscan, hashcheck, quiz, base64, portscan, bruteforce, detect";
  }
  else if(cmd.startsWith("caesar")){
    int space1 = cmd.indexOf(' ');
    int space2 = cmd.indexOf(' ', space1+1);
    if(space1>0 && space2>0){
      String text = cmd.substring(space1+1, space2);
      int shift = cmd.substring(space2+1).toInt();
      String out="";
      for(int i=0;i<text.length();i++){
        char c = text[i];
        if(c>='A' && c<='Z') out += char((c-'A'+shift)%26+'A');
        else if(c>='a' && c<='z') out += char((c-'a'+shift)%26+'a');
        else out+=c;
      }
      resp="Chiffrement César: "+out;
    } else resp="Usage: caesar texte decalage";
  }
  else if(cmd.startsWith("rot13")){
    String text = cmd.substring(6);
    String out="";
    for(int i=0;i<text.length();i++){
      char c = text[i];
      if(c>='A' && c<='Z') out += char((c-'A'+13)%26+'A');
      else if(c>='a' && c<='z') out += char((c-'a'+13)%26+'a');
      else out+=c;
    }
    resp="ROT13: "+out;
  }
  else if(cmd.startsWith("pwdcheck")){
    String pwd = cmd.substring(8);
    if(pwd.length()<6) resp="❌ Trop court";
    else if(pwd.indexOf("123")>=0 || pwd.indexOf("password")>=0) resp="⚠️ Faible";
    else resp="✅ Fort";
  }
  else if(cmd=="ipscan"){
    resp="IP fictives:\n192.168.1.1 alive\n192.168.1.2 dead\n192.168.1.3 alive";
  }
  else if(cmd=="hashcheck"){
    resp="SHA1 fictif: "+String(random(100000,999999));
  }
  else if(cmd=="quiz"){
    resp="Question: Quel outil Steam permet de lancer les jeux Windows sur Linux ?\nRéponse: proton";
  }
  else if(cmd.startsWith("base64")){
    String txt = cmd.substring(7);
    String enc="";
    for(int i=0;i<txt.length();i++){
      enc += String(txt[i], HEX);
    }
    resp="Base64 (simulé hex): "+enc;
  }
  else if(cmd=="portscan"){
    resp="Ports fictifs ouverts: 22, 80, 443";
  }
  else if(cmd=="bruteforce"){
    resp="Simulation brute-force: mot de passe faible trouvé en 5 essais";
  }
  else if(cmd=="detect"){
    resp="Analyse chaîne: aucun pattern suspect détecté";
  }
  else{
    resp="Commande inconnue, tapez 'help'";
  }

  logs += "> "+cmd+"\n"+resp+"\n";
  if(logs.length()>8000) logs.remove(0,2000);
  return resp;
}

// Routes
void handleRoot(){ server.send(200,"text/html",htmlPage); }
void handleExec(){
  if(server.hasArg("cmd")){
    String cmd = server.arg("cmd");
    server.send(200,"text/plain",handleCommand(cmd));
  } else server.send(400,"text/plain","Commande manquante");
}

void setup(){
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid,password);
  Serial.println("ESP32 Cyber Lab avancé lancé !");
  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("Mot de passe: "); Serial.println(password);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/exec", handleExec);
  server.begin();
}

void loop(){ server.handleClient(); }
