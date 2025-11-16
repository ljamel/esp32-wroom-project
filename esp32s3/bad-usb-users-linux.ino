#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

void setup() {
  // Démarrer interface USB HID clavier
  USB.begin();
  Keyboard.begin();

  delay(30);  // attente pour te laisser cliquer sur le terminal

  // Texte à taper j'ai converti azerty en querty pour que ca fonctionne sur le clavier fr = cat /etc/passwd
  const char *texte = "aaaaaaaaaa cqt >etc>pqsszd";

  // Taper le texte caractère par caractère
  for (int i = 0; texte[i] != '\0'; i++) {
    Keyboard.write(texte[i]);  // write gère mieux les codes de touches
    delay(80);
  }
  

  Keyboard.write(KEY_RETURN);

cat /etc/passwd


}

void loop() {
  // Ne rien faire
}
