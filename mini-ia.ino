// ESP32 IA "Chatbot basique" tout-en-un
#include <Arduino.h>
#include <string.h>

const char* keywords[] = {
  "bonjour", "salut", "hello", "aide", "merci", "au revoir", "bye", "comment", 
  "ça va", "heure", "temps", "nom", "âge", "ville", "pays", "ordinateur", "ESP32",
  "musique", "film", "sport", "foot", "basket", "tennis", "nourriture", "pizza",
  "chien", "chat", "animal", "vacances", "été", "hiver", "printemps", "automne",
  "voiture", "train", "avion", "bateau", "livre", "lecture", "histoire", "science",
  "math", "programmation", "code", "IA", "robot", "jeu", "ordinateur", "technologie",
  "planète", "univers", "école"
};

const char* responses[] = {
  "Salut !", "Salut !", "Hello !", "Je peux t'aider.", "De rien !", "À bientôt !", 
  "Bye !", "Ça va bien, merci.", "Je vais bien.", "Il est midi.", "Le temps est beau.", 
  "Je m'appelle ESP32-IA.", "Je suis un ESP32.", "J'adore la musique.", "J'aime les voyages.", 
  "Je fonctionne sur un ESP32.", "Oui, je suis un microcontrôleur.", "J'aime la musique.", 
  "Je regarde parfois des films.", "J'aime le sport.", "Le foot est populaire.", 
  "Le basket est amusant.", "Le tennis est sympa.", "J'aime la nourriture.", "La pizza est délicieuse.", 
  "J'aime les chiens.", "Les chats sont mignons.", "Les animaux sont fascinants.", 
  "Les vacances sont relaxantes.", "J'aime l'été.", "L'hiver est froid.", "Le printemps est agréable.", 
  "L'automne est coloré.", "J'aime les voitures.", "Le train est rapide.", "L'avion vole haut.", 
  "Le bateau navigue sur l'eau.", "J'adore lire des livres.", "La lecture est enrichissante.", 
  "L'histoire est passionnante.", "La science est incroyable.", "Les mathématiques sont utiles.", 
  "J'aime programmer.", "Le code est amusant.", "L'IA est fascinante.", "Les robots sont cool.","jouons ensemble", 
  "J'utilise un ordinateur.", "La technologie évolue vite.", "La planète Terre est belle.", 
  "L'univers est immense.", "L'école est importante."
};

// Définir numResponses automatiquement
const int numResponses = sizeof(keywords)/sizeof(keywords[0]);


// --- Fonction améliorée pour correspondance partielle ---
const char* getResponse(const char* input) {
  char buf[128];
  strncpy(buf, input, sizeof(buf)-1);
  buf[127] = 0; // sécurité

  // mettre en minuscules
  for (int i = 0; buf[i]; i++) {
    if (buf[i] >= 'A' && buf[i] <= 'Z') buf[i] += 32;
  }

  // recherche partielle
  for (int i = 0; i < numResponses; i++) {
    const char* key = keywords[i];
    char keybuf[64];
    strncpy(keybuf, key, sizeof(keybuf)-1);
    keybuf[63] = 0;
    // minuscules pour la clé
    for (int j = 0; keybuf[j]; j++) {
      if (keybuf[j] >= 'A' && keybuf[j] <= 'Z') keybuf[j] += 32;
    }

    if (strstr(buf, keybuf) != NULL) {
      return responses[i];
    }
  }
  return "Désolé, je ne comprends pas 😅";
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-IA est prêt !");
}

void loop() {
  static char input[128];
  if (Serial.available() > 0) {
    int len = Serial.readBytesUntil('\n', input, sizeof(input)-1);
    input[len] = 0; // fin de chaîne
    const char* reply = getResponse(input);
    Serial.println(reply);
  }
}
