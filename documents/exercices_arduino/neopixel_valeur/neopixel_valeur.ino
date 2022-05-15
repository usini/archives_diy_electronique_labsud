/*
 * Clignoter.ino
 * Faire clignoter une led neopixel (WS2812b)
 */

//Nécessite la bibliothèque : FastLED
#include <FastLED.h>

const int NUM_LEDS = 2; //Nombre de leds
const int LEDS_PIN = 6; //Broche où sont branchés les leds
int valeur = 0;
int sens = true;
// Tableau des leds (où est stocker leur état)
CRGB leds[NUM_LEDS];

void setup() {
  //Configuration des leds Neopixel (fastleds gère plusieurs types de leds)
  FastLED.addLeds<NEOPIXEL, LEDS_PIN>(leds, NUM_LEDS);
  
  //Il est possible de limiter la consommation électrique des leds
  //(5 <-- Volts, 10 <-- Milliampère)
  //C'est surtout utile si vous voulez contrôler des strips de leds de plus de 5 leds
  //Sans alimentation externe.
  //https://github.com/FastLED/FastLED/wiki/Power-notes
  FastLED.setMaxPowerInVoltsAndMilliamps(5,10);
}

void loop() {

  if(valeur == 255){
    sens = false;
  }
  
  if(valeur == 0){
    sens = true;
  }
  
  if(sens){
    valeur++;
  } else {
    valeur--;
  }
  
  //On parcours les leds
  for (int led, led < NUM_LEDS; led++){
    leds[led] = CHSV(valeur, 255, 255);
  }
  //Après avoir modifier l'état des leds, on applique le changement avec cette commande
  FastLED.show(); 
  delay(5);
  
}

  //Pour changer la couleur d'une led nous allons utiliser la norme HSV
  //https://raw.github.com/FastLED/FastLED/gh-pages/images/HSV-rainbow-with-desc.jpg
  //HSV prends 3 valeurs
  //Hue : la couleur
  //Saturation : A quel point la couleur va "tirer" vers le blanc (0 blanc --> 255 couleur)
  //Value : La luminosité de la led

  //Par ex pour changer la couleur de la première leds
  //leds[0] = CHSV(HUE, SATURATION, VALUE);  
  //L'important est surtout la première valeur.

        
 
