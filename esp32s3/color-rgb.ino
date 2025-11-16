#include <Adafruit_NeoPixel.h>

#define LED_PIN 48
#define NUM_PIXELS 1

Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pixels.setBrightness(55);  // réglage intensité max
}

void loop() {
  // Rouge
  pixels.setPixelColor(0, pixels.Color(200, 80, 100));
  pixels.show();
  delay(1000);

  // Vert
  pixels.setPixelColor(0, pixels.Color(80, 211, 50));
  pixels.show();
  delay(1000);

  // Bleu
  pixels.setPixelColor(0, pixels.Color(100, 0, 200));
  pixels.show();
  delay(1000);

  // Blanc (RGB)
  pixels.setPixelColor(0, pixels.Color(255, 255, 255));
  pixels.show();
  delay(1000);

  pixels.clear();
  pixels.show();
  delay(500);
}
