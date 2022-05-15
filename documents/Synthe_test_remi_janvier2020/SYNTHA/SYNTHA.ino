// Arduino synth library MIDI example
//
// The arduino receives MIDI over Serial at 31250 BAUD
//
// Each of the four voices are assigned to MIDI channel 1,2,3 and 4
// For each channel:
// NOTE_ON with velocity over zero triggers voice.
// Controller 13 controls waveform: SINE[0-21] TRIANGLE[22-42] SQUARE[43-63] SAW[64-84] RAMP[85-105] NOISE[106-127]
// Controller 12 controls envelope: ENVELOPE0[0-32] ENVELOPE1[33-64] ENVELOPE2[65-96] ENVELOPE3[97-127]
// Controller 10 controls length: [0-127]
// Controller 7  controls modulation: negative[0-63] neutral[64] positive[65-127]
//


//Hardware connections:

//                    +10µF
//PIN 11 ---[ 1k ]--+---||--->> Audio out
//                  |
//                 === 10nF
//                  |
//                 GND

// DZL 2014
// HTTP://dzlsevilgeniuslair.blogspot.dk
// HTTP://illutron.dk

#include <synth.h>

synth edgar;        //-Make a synth
const int NB_BUTTONS = 4;
const int BUTTON_PINS[NB_BUTTONS] = {5, 6, 7, 8};
const uint8_t NOTES[NB_BUTTONS] = {56, 59, 61, 65};
int note_playing = 0;
bool button_pressed[4] = {false, false, false, false};
int pitch = 0;
void setup()
{
  Serial.begin(115200);    //MIDI BAUD rate
  edgar.begin();          //Init synth
  edgar.setupVoice(0, RAMP, 127, ENVELOPE0, 110, 64);
  edgar.setupVoice(1, RAMP, 127, ENVELOPE1, 110, 64);
  edgar.setupVoice(2, RAMP, 127, ENVELOPE2, 110, 64);
  edgar.setupVoice(3, RAMP, 127, ENVELOPE3, 110, 64);
  for (int i = 0; i < NB_BUTTONS; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }
}

void loop()
{
  /*
  int new_pitch = analogRead(A3) >> 2;
  if(pitch != new_pitch){
     pitch = new_pitch;
     for(int i=0;i<4;i++){
      edgar.setPitch(i, new_pitch);
     }
  }
  */
  for(int i=0;i<4;i++){
    edgar.setWave(i,analogRead(A1) >> 7);
  edgar.setLength(i, analogRead(A2) >> 3);
  //edgar.setMod(i, analogRead(A2) >> 3);
  }
  for (int i = 0; i < NB_BUTTONS; i++) {
    if (digitalRead(BUTTON_PINS[i])) {
      if (button_pressed[i]) {
        button_pressed[i] = false;
      }
    } else {
      if (!button_pressed[i]) {
        Serial.print("LENGTH:");
        Serial.print(analogRead(A1) >> 3);
        Serial.print(" MOD:");
        Serial.print(analogRead(A2) >> 3);
        Serial.print(" NOTE:");
        button_pressed[i] = true;
        edgar.mTrigger(note_playing, NOTES[i]);
        Serial.println(NOTES[i]);
        note_playing++;
        if (note_playing == 4) {
          note_playing = 0;
        }
      }
    }
  }

  /*
    while(Serial.available())
    {
    if(parser.update(Serial.read()))  //-Feed MIDI stream to parser and execute commands
    {
      switch(parser.midi_cmd)
      {
        //*********************************************
        // Handle MIDI notes
        //*********************************************
      case 0x90: //-Channel 1 (voice 0)
      case 0x91: //-Channel 2 (voice 1)
      case 0x92: //-Channel 3 (voice 2)
      case 0x93: //-Channel 4 (voice 3)

        voice = parser.midi_cmd-0x90;
        if(parser.midi_2nd)  //-Velocity not zero (could implement NOTE_OFF here)
          edgar.mTrigger(voice,parser.midi_1st);
        break;

        //*********************************************
        // Handle MIDI controllers
        //*********************************************
      case 0xb0:  //-Channel 1 (voice 0)
      case 0xb1:  //-Channel 2 (voice 1)
      case 0xb2:  //-Channel 3 (voice 2)
      case 0xb3:  //-Channel 4 (voice 3)
        voice=parser.midi_cmd-0xb0;
        switch(parser.midi_1st)  //-Controller number
        {
        case 13:  //-Controller 13
          edgar.setWave(voice,parser.midi_2nd/21);
          break;
        case 12:  //-Controller 12
          edgar.setEnvelope(voice,parser.midi_2nd/32);
          break;
        case 10:  //-Controller 10
          edgar.setLength(voice,parser.midi_2nd);
          break;
        case 7:   //-Controller 7
          edgar.setMod(voice,parser.midi_2nd);
          break;
        }
        break;
      }
    }
    }
  */
}
