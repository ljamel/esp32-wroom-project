#include <WiFi.h>

const char* ssid = "Ton_SSID";
const char* password = "Ton_MotDePasse";

WiFiServer telnetServer(23);  // Port 23 pour Telnet
WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  Serial.println("WiFi connecté !");
  telnetServer.begin();
  telnetServer.setNoDelay(true);
  Serial.print("Telnet prêt sur IP : ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (telnetServer.hasClient()) {
    if (!client || !client.connected()) {
      client.stop();
      client = telnetServer.available();
      client.println("Connexion Telnet ESP32 OK !");
    }
  }

  if (client && client.connected() && client.available()) {
    String cmd = client.readStringUntil('\n');
    cmd.trim();

  
  	if (cmd == "GETENC") {
	  // Ex3 : XOR challenge (hex)
	  client.println("U		YXE");
	  client.println("Hint: clé 2 octets. Réponse via commande ANS_XOR <mot> (ex: ARZ_ROX 3e)");
	} else if (cmd == "ANS_XOR bonjour") {
	  client.println("FLAG{xored_flag}");
	} else if (cmd == "GETBASE64") {
	  // Ex4 : base64 via HTTP simulation
	  client.println("RkxBR3todHRwX2Jhc2U2NF9kZWNvZGV9");
	  client.println("Décoder et renvoyer via ANS_BASE64 <flag>");
	} else if (cmd == "ANS_BASE64 FLAG{http_base64_decode}") {
	  client.println("FLAG{http_base64_decode}");
	} else if (cmd == "CTF_MD5") {
	  // Ex2 : MD5 hash (common)
	  client.println("md5: 5f4dcc3b5aa765d61d8327deb882cf99");
	  client.println("Trouve le mot de passe et renvoie via ANS_MD5 <password>");
	} else if (cmd == "ANS_MD5 password") {
	  client.println("FLAG{password_cracked}");
	} else if (cmd == "AES_TEST") {
	  // Ex5 : AES-CBC example (IV/key donnés)
	  client.println("IV: 00112233445566778899aabbccddeeff");
	  client.println("CT: 8ea2b7ca516745bfeafc49904b496089");
	  client.println("Clé (dans firmware): 00112233445566778899aabbccddeeff");
	  client.println("Déchiffre et renvoie via ANS_AES <flag>");
	} else if (cmd == "ANS_AES FLAG{aes_flag}") {
	  client.println("FLAG{aes_flag}");
	} else if (cmd == "RIDDLE1") {
	  // Devinette 1 (français)
	  client.println("Devinette: Je suis toujours devant toi mais on ne peut jamais m'atteindre. Qui suis-je ?");
	  client.println("Répond via ANS_R1 <réponse> (sans accents si tu veux).");
	} else if (cmd == "ANS_R1 lavenir" || cmd == "ANS_R1 l'avenir") {
	  client.println("FLAG{avenir_found}");
	} else if (cmd == "RIDDLE2") {
	  client.println("Devinette: Je parle toutes les langues, je n'ai pas de bouche. Qui suis-je ?");
	  client.println("Répond via ANS_R2 <réponse>.");
	} else if (cmd == "ANS_R2 echo" || cmd == "ANS_R2 l'echo" || cmd == "ANS_R2 lecho") {
	  client.println("FLAG{echo_master}");
	} else if (cmd == "LINUX1") {
	  // Question Linux simple
	  client.println("Question Linux: Quelle commande affiche la liste des fichiers d'un répertoire ?");
	  client.println("Répond via ANS_L1 <commande> (ex: ANS_L1 ls)");
	} else if (cmd == "ANS_L1 ls") {
	  client.println("FLAG{linux_ls}");
	} else if (cmd == "LINUX2") {
	  client.println("Question Linux: Quelle commande affiche les processus en cours ?");
	  client.println("Répond via ANS_L2 <commande> (ex: ANS_L2 ps)");
	} else if (cmd == "ANS_L2 ps") {
	  client.println("FLAG{linux_ps}");
	} else if (cmd == "ROT13") {
	  // ROT13 challenge
	  client.println("Token (rot13): SYNAT{tbg_gb_arg}");
	  client.println("Décoder et renvoyer via ANS_ROT13 <flag>");
	} else if (cmd == "ANS_ROT13 FLAG{got_to_net}") {
	  client.println("FLAG{got_to_net}");
	} else if (cmd == "HELP") {
	  client.println("Commands: GETFLAG1, AUTH <pw>, GETENC, ANS_XOR <hexkey>, GETBASE64, ANS_BASE64 <flag>,");
	  client.println("          CTF_MD5, ANS_MD5 <pw>, AES_TEST, ANS_AES <flag>, RIDDLE1, ANS_R1 <resp>, RIDDLE2, ANS_R2 <resp>,");
	  client.println("          LINUX1, ANS_L1 <cmd>, LINUX2, ANS_L2 <cmd>, ROT13, ANS_ROT13 <flag>");
	} else {
	  client.println("Unknown cmd");
	}

  }
}
