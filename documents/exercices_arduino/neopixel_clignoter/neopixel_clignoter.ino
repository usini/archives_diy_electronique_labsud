#include <FastLED.h>

const int NUM_LEDS = 1; //Nombre de leds
const int LEDS_PIN = 6; //Broche où sont branchés les leds

CRGB leds[NUM_LEDS]; //Tableau des leds

void setup() {
  //Initalisation des leds
  FastLED.addLeds<NEOPIXEL, LEDS_PIN>(leds, NUM_LEDS);
}

void loop() {
  leds[0] = CHSV(192, 255, 255);
  FastLED.show();
  delay(1000);
 
  leds[0] = CHSV(0, 0, 0);
  FastLED.show();
  delay(1000);
}
