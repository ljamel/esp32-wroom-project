#include "USB.h"
#include "USBHIDKeyboard.h"

# bad usb simple inofencif pour apprendre
# envoi un texte dans le terminal

USBHIDKeyboard Keyboard;

void setup() {
  // Démarrer interface USB HID clavier
  USB.begin();
  Keyboard.begin();

  delay(300);  // attente pour te laisser cliquer sur le terminal

  // Texte à taper ci-dessous les première lettre son ignorer penser a trouver une solution..
  const char *texte = "aaaaaaaaaaaaaa azerty ou qwerty";

  // Taper le texte caractère par caractère
for (int i = 0; texte[i] != '\0'; i++) {
  Keyboard.write(texte[i]);  // write gère mieux les codes de touches
  delay(80);
}


}

void loop() {
  // Ne rien faire
}
