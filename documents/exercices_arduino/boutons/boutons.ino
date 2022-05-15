// Buttons avec debounce
/*
  Debouncer des boutons nous permet d'éviter les faux positifs
  Et ne pas arrêter le programme pour lire les boutons avec un delay
  Ici on exécute une action uniquement si l'état du bouton change.

  On utilise des tableaux pour stocker les boutons, ainsi on peut 
  rapidement changer le nombre des boutons et où ils sont branchés
*/

/* BOUTTONS */
int nb_buttons = 4;
int buttons_pins[4] = {3, 4, 7, 6}; //Broches où sont branchés les boutons
unsigned long buttons_debounce[4] = {0, 0, 0, 0}; //Temps depuis le dernier changement d'état
unsigned long debounce_delay = 15; //Délai avant un changement d'état
bool buttons_pressed[4] = {false, false, false, false}; //Etat des boutons

void setup() {
  Serial.begin(9600);
  setup_buttons();
}

void loop() {
  read_buttons();
}

void action_button_pressed(int button) {
  Serial.print(buttons_pins[button]);
  Serial.println(":1");
}

void action_button_unpressed(int button) {
  Serial.print(buttons_pins[button]);
  Serial.println(":0");
}

void setup_buttons() {
  //Initialisation des boutons
  for (int button = 0; button < nb_buttons; button++) {
    pinMode(buttons_pins[button], INPUT_PULLUP);
  }
}

void read_buttons() {
  //On Parcours les boutons
  for (int button = 0; button < nb_buttons; button++) {
    //Logique inversé à cause du pullup
    bool button_state = !digitalRead(buttons_pins[button]);

    //Si l'état du bouton est identique
    if (button_state == buttons_pressed[button]) {
      // On remet à zéro le compteur (faux positif)
      buttons_debounce[button] = millis();
    }

    //Si le bouton est enclenché/déenclenché depuis plus de 15ms (debounce_delay)
    if ((millis() - buttons_debounce[button]) > debounce_delay) {

      //Si le bouton n'est pas appuyé ...
      if (!button_state) {
        // Et que son état précédent est appuyé
        if (buttons_pressed[button]) {
          buttons_pressed[button] = false;
          action_button_unpressed(button);
        }
      }

      // Si le bouton est appuyé ...
      if (button_state) {
        // Et son état précédent est non appuyé
        if (!buttons_pressed[button]) {
          buttons_pressed[button] = true;
          action_button_pressed(button);
        }
      }
    }
  }
}
