
/*
    BLE Midi 8 Buttons
    by Rémi Sarrailh
    by Olivier Sarrailh

    Based on BLE_MIDI Example by neilbags
    https://github.com/neilbags/arduino-esp32-BLE-MIDI
    Based on BLE_notify example by Evandro Copercini.
    Creates a BLE MIDI service and characteristic.
    Once a client subscibes, press a button to play a midi note
    //Todo get CC commands
    //Essentials of the Midi Protocol https://ccrma.stanford.edu/~craig/articles/linuxmidi/misc/essenmidi.html
    //Note to hex (https://www.wavosaur.com/download/midi-note-hex.php)
    //http://chephip.free.fr/mus/midi.html
*/

/* BLUETOOTH */
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define MIDI_SERVICE_UUID        "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;


/* BUTTONS */
int buttons_pins[8] = {13, 15, 2, 0, 4, 16, 17, 5};
int buttons_note[7] =     {60, 62, 64, 65, 67, 69, 71};
int buttons_note_alt[7] = {61, 63, 65, 66, 68, 70, 72};
unsigned long buttons_debounce[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned long debounce_delay = 10;
bool buttons_pressed[8] = {true, true, true, true, true, true, true, true};
bool alt_toggle = false;
const int nb_buttons = 8;


/* Onboard Led */
unsigned long ledTimer = 0;
const long ledSpeed = 300;
bool ledState = false;
const int ledPin = 22;

/* MIDI */
#define NOTE_OFF 0x80
#define NOTE_ON 0x90
#define AFTERTOUCH 0xA0
#define PITCH_BEND 0xE0

uint8_t midiPacket[] = {
  0x80,  // header
  0x80,  // timestamp, not implemented
  0x00,  // commands
  0x3c,  // 0x3c == 60 == middle c
  0x00   // velocity
};

//midiPacket[0] : Header
//midiPacket[1] : TimeStamp
//midiPacket[2] : Status
//midiPacket[3] : Note
//midiPacket[4] : Velocity

// https://www.wavosaur.com/download/midi-note-hex.php
// 0x30 C3

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Connected");
      deviceConnected = true;
      digitalWrite(ledPin, LOW); //Invert logic!
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Disconnected");
      deviceConnected = false;
      digitalWrite(ledPin, HIGH);
    }
};

void blinkLed() {
  unsigned long currentMillis = millis();
  if (currentMillis - ledTimer >= ledSpeed) {
    ledTimer = currentMillis;
    if (ledState) {
      //Serial.println("ON");
      digitalWrite(ledPin, HIGH);
      ledState = false;
    } else {
      //Serial.println("OFF");
      digitalWrite(ledPin, LOW);
      ledState = true;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); //Turn off led (invert logic)

  for (int button = 0; button < nb_buttons; button++) {
    if (!digitalRead(buttons_pins[button])) {
      pinMode(buttons_pins[button], INPUT_PULLUP);
    }
    Serial.println(buttons_pins[button]);
  }


  /* BLUETOOTH */
  BLEDevice::init("Flute");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEDevice::setEncryptionLevel((esp_ble_sec_act_t)ESP_LE_AUTH_REQ_SC_BOND);

  // Create the BLE Service
  BLEService *pService = pServer->createService(BLEUUID(MIDI_SERVICE_UUID));

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      BLEUUID(MIDI_CHARACTERISTIC_UUID),
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_WRITE_NR
                    );
  pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
  pSecurity->setCapability(ESP_IO_CAP_NONE);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

  pServer->getAdvertising()->addServiceUUID(MIDI_SERVICE_UUID);
  pServer->getAdvertising()->start();
}

void loop() {
  if (deviceConnected) {
    buttons_read();
  } else {
    blinkLed();
  }
}

void buttons_read() {
  for (int button = 0; button < nb_buttons; button++) {
    //delay(30);

    bool button_state = !digitalRead(buttons_pins[button]); //Invert because pullup.
    //Serial.print(button_state);
    // If the switch changed, due to noise or pressing:
    if (button_state != buttons_pressed[button]) {
      // reset the debouncing timer
      buttons_debounce[button] = millis();
    }

    if ((millis() - buttons_debounce[button]) > debounce_delay) {
      if (!button_state) { //If buttons is not pressed
        // If buttons wasn't pressed
        if (!buttons_pressed[button]) {
          buttons_pressed[button] = true;
          Serial.print(button);
          Serial.println(":OFF");
          //Todo Refactor to know which note to stop
          if (button != 7) {
            sendMidi(NOTE_OFF, buttons_note[button]);
            sendMidi(NOTE_OFF, buttons_note_alt[button]);
          } else {
            if (button == 7) {
              for (int i = 0; i < nb_buttons; i++) {
                if (i != 7) {
                  if (!buttons_pressed[i]) {
                    sendMidi(NOTE_OFF, buttons_note_alt[i]);
                    sendMidi(NOTE_ON, buttons_note[i]);
                  }
                }
              }
            }
          }
        }
      }
      if (button_state) { // If button is pressed
        // If buttons was already pressed
        if (buttons_pressed[button]) {
          buttons_pressed[button] = false;
          Serial.print(button);
          Serial.println(":ON");

          if (button != 7) {
            if (buttons_pressed[7]) {
              sendMidi(NOTE_ON, buttons_note[button]);
            } else {
              sendMidi(NOTE_ON, buttons_note_alt[button]);
            }
          } else {
            for (int i = 0; i < nb_buttons; i++) {
              if (i != 7) {
                if (!buttons_pressed[i]) {
                  sendMidi(NOTE_OFF, buttons_note[i]);
                  sendMidi(NOTE_ON, buttons_note_alt[i]);
                }
              }
            }
          }
        }
      }
    }
  }
}

void sendMidi(int type, int note) {
  midiPacket[2] = type; // note down, channel 0
  midiPacket[3] = note;
  midiPacket[4] = 127;  // velocity
  pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
  pCharacteristic->notify();
}