#include "MIDIUSB.h"

/* Settings for MIDI preset */
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
const int NB_BUTTONS = ROWS * COLS;

const int PRESET = 8;
String preset_name[PRESET] = {"Chill", "Ambiant", "Arpeggiator", "Sustain Piano", "Bells/Percs", "3xOSC", "Guitar/Drums", "MegaDelay"};
int notes[PRESET][NB_BUTTONS] = {
  {49, 45, 50, 52, 50, 55, 58, 60, 62, 63, 65, 67, 0, 62, 67, 65}, //Preset 1
  {74, 69, 67, 70, 69, 62, 71, 66, 39, 61, 59, 58, 0, 66, 60, 63}, // Preset 2
  {35, 39, 42, 45, 48, 51, 54, 57, 62, 64, 65, 67, 0, 70, 72, 77}, // Preset 3
  {50, 57, 58, 60, 62, 65, 69, 72, 74, 77, 81, 86, 0, 89, 91, 93},  //Preset A
  {60, 67, 63, 70, 62, 75, 57, 72, 63, 67, 60, 65, 0, 67, 62, 72},  //Preset 4
  {58, 60, 65, 67, 63, 72, 58, 70, 84, 86, 87, 91, 0, 96, 99, 103},  //Preset 5
  {127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 0, 114, 113, 112},  //Preset 6
  {60, 63, 68, 65, 60, 67, 72, 70, 68, 74, 75, 77, 0, 63, 74, 7}  //Preset B
};

int channel[PRESET][NB_BUTTONS] = {
  {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2}, //Preset 1
  {3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 0, 5, 5, 5}, //Preset 2
  {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 6, 6, 6}, //Preset 3
  {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7}, //Preset A
  {8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 0, 10, 10, 10}, //Preset 4
  {11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 0, 12, 12, 12}, //Preset 5
  {13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 13, 13, 13}, //Preset 6
  {14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 0, 15, 15, 15}, //Preset B
};

/* Keypad Init */
//https://github.com/Nullkraft/Keypad
#include <Keypad.h>
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

//Keypad pins
byte rowPins[ROWS] = {8, 9, 10, 11}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the kpd
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Keypad state : 1 = DOWN 3 = UP (there are other state which are unused here)
unsigned int buttons_state[NB_BUTTONS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool preset_selector = false;

//Oled Display
#include <Wire.h> //I2C
#include <U8g2lib.h>
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
String oled_line1;
String oled_line2;

//Preset
int selected_preset = 0;

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  change_preset(selected_preset);
}

//Change preset and display it on screen
//You need to press "*" again to validate the preset
void change_preset(int preset) {
  if (preset < PRESET) {
    selected_preset = preset;
    oled_line1 = "-----Preset------";
    oled_line2 = preset_name[preset];
    printScreen();
    Serial.println(oled_line1);
    Serial.println(oled_line2);
  }
}

//Change what is written on the Oled screen using global variable
//oled_line1 and oled_line2
void printScreen() {
  //https://github.com/olikraus/u8g2/wiki/fntlist8x8
  u8g2.setFont(u8g2_font_7x14_mr);
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 14);
    u8g2.println(oled_line1);  // write something to the internal memory
    u8g2.setCursor(0, 30);
    u8g2.println(oled_line2);  // write something to the internal memory
  }
  while ( u8g2.nextPage() );
}

void loop() {
  if (kpd.getKeys()) {
    for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        buttons_state[kpd.key[i].kcode] = kpd.key[i].kstate;

        if (kpd.key[i].kstate == 1) { //DOWN
          Serial.print(kpd.key[i].kcode);
          Serial.print(":");
          Serial.println("1");

          //If key "#" is pressed
          if (kpd.key[i].kcode == 12) {
            //If preset selection is not active, we go to selection mode
            if (!preset_selector) {
              Serial.println("Selector");
              preset_selector = true;
              oled_line1 = "Selector";
              oled_line2 = "--------";
              printScreen();
            } else {
              //If preset selection is active, we return to midi mode
              Serial.println("Deselect");
              oled_line1 = preset_name[selected_preset];
              oled_line2 = "";
              printScreen();
              preset_selector = false;
            }
          } else {
            //If in selection mode we change preset
            if (preset_selector) {
              Serial.println("Change preset");
              change_preset(kpd.key[i].kcode);
            } else {
              //If in midi mode we play midi note
              key_down(kpd.key[i].kcode);
            }
          }
        }
        if (kpd.key[i].kstate == 3) { //UP
          Serial.print(kpd.key[i].kcode);
          Serial.print(":");
          Serial.println("0");
          // If key is up we send a midi note off
          key_up(kpd.key[i].kcode);
        }
      }
    }
  }
}

//Key down (pressed)
void key_down(int key) {
  noteOn(channel[selected_preset][key], notes[selected_preset][key], 100);
  //Display note and velocity on screen
  oled_line1 = preset_name[selected_preset];
  oled_line2 = "Note:" + String(notes[selected_preset][key]) + " " "C:" + String(channel[selected_preset][key]);
  printScreen();
  MidiUSB.flush();
}

//Key up (unpressed)
void key_up(int key) {
  noteOff(channel[selected_preset][key], notes[selected_preset][key], 127);
  MidiUSB.flush();
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
