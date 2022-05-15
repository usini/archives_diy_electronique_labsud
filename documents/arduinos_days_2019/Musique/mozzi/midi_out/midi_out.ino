#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos1024_int8.h> // table for Oscils to play
#include <tables/saw1024_int8.h>
#include <tables/triangle1024_int8.h>
#include <tables/square_no_alias512_int8.h>
#include <mozzi_midi.h> // for mtof
#include <mozzi_fixmath.h>
#include <EventDelay.h>
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();
#include <SoftwareSerial.h>
SoftwareSerial softSerial(2, 3);

Oscil<COS1024_NUM_CELLS, AUDIO_RATE> aCos(COS1024_DATA);
Oscil<SAW1024_NUM_CELLS, AUDIO_RATE> bSaw(SAW1024_DATA);
Oscil<TRIANGLE1024_NUM_CELLS, AUDIO_RATE> cTri(TRIANGLE1024_DATA);
Oscil<SQUARE_NO_ALIAS512_NUM_CELLS, AUDIO_RATE> dsqua(SQUARE_NO_ALIAS512_DATA);
int notemidi = 0;
bool onoff = false;
MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial, midiA);

void handleNoteOn(byte channel, byte pitch, byte velocity) {
  //Serial.print("NoteOn:");
  onoff = true;
  notemidi = pitch;
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  onoff = false;
  //Serial.print("NoteOff:");
  //Serial.println(pitch);
}

void setup() {
  //Serial.begin(115200);
  midiA.setHandleNoteOn(handleNoteOn);
  midiA.setHandleNoteOff(handleNoteOff);
  midiA.begin(MIDI_CHANNEL_OMNI);
  startMozzi();
}

void loop() {
  audioHook();
   midiA.read();
}

int updateAudio() {
  if (onoff) {
    return aCos.next();
  }
}


void updateControl() {
 
  // convertion en fréquence de la note midi
  int freq = mtof(notemidi); //note midi en frequence
  // fréquence de lecture de la fonction
  aCos.setFreq(freq);
  //bSaw.setFreq(freq);
  //cTri.setFreq(freq);
  //dsqua.setFreq(freq);
}
