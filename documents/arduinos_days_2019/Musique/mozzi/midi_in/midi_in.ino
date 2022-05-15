//MIDI
#include <SoftwareSerial.h>
SoftwareSerial midi(8,9);
void setup() {
  // put your setup code here, to run once:
  midi.begin(57600);
  Serial.begin(57600);
}

void loop() {
  noteOn(0x90, 0x50, 0x45);
  delay(10);
  Serial.println("Sent");
  // put your main code here, to run repeatedly:
}

void noteOn(byte cmd, byte pitch, byte velocity) {
  midi.write(cmd);
  midi.write(pitch);
  midi.write(velocity);
}
