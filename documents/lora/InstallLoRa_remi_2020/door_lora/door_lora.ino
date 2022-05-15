#include <SPI.h>
#include <TinyLoRa.h>
#include "secrets.h"
#include <LowPower.h>


int sleepcycles = 38; //5minutes (8s*38)
#define VBATPIN A9
const int reedSwitch1 = 2; //LOCK
const int reedSwitch2 = 3; //DOOR
#define LedPin LED_BUILTIN
// Visit your thethingsnetwork.org device console
// to create an account, or if you need your session keys.

/************************** Example Begins Here ***********************************/
// Pinout for Adafruit Feather M0 LoRa
//TinyLoRa lora = TinyLoRa(3, 8);

// Data Packet to Send to TTN
unsigned char loraData[4];

// How many times data transfer should occur, in seconds
const unsigned int sendInterval = 30;

// Pinout for Adafruit Feather 32u4 LoRa
TinyLoRa lora = TinyLoRa(7, 8, 4);

// Pinout for Adafruit Feather M0 LoRa
//TinyLoRa lora = TinyLoRa(3, 8);


// Pinout for Adafruit Feather M0 LoRa
//TinyLoRa lora = TinyLoRa(3, 8);

void setup()
{
  Serial.begin(9600);
  //while (!Serial);
  unsigned status;
  // Initialize pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(reedSwitch1, INPUT);
  pinMode(reedSwitch2, INPUT);
  attachInterrupt(0, wake_and_send, CHANGE);
  attachInterrupt(1, wake_and_send, CHANGE);

  // Initialize LoRa
  Serial.print("Starting LoRa...");
  // define multi-channel sending
  lora.setChannel(CH0);
  // set datarate
  lora.setDatarate(SF7BW125);
  if (!lora.begin())
  {
    Serial.println("Failed");
    Serial.println("Check your radio");
    while (true);
  }

}

void wake_and_send() {
  sendDoor();
}

void loop()
{
  sendDoor();

}

void sendDoor() {
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  int16_t vbat = measuredvbat * 100;
  // Negative Values : https://www.thethingsnetwork.org/forum/t/negative-temperature/21330/3
  // Encoding to bytes :  https://www.thethingsnetwork.org/docs/devices/bytes.html

  // encode int as bytes
  //byte payload[2];
  loraData[0] = digitalRead(reedSwitch1);
  loraData[1] = digitalRead(reedSwitch2);
  loraData[2] = highByte(vbat);
  loraData[3] = lowByte(vbat);

  Serial.println("Sending LoRa Data...");
  lora.sendData(loraData, sizeof(loraData), lora.frameCounter);
  Serial.print("Frame Counter: "); Serial.println(lora.frameCounter);
  lora.frameCounter++;

  // blink LED to indicate packet sent
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.print(digitalRead(reedSwitch1));
  Serial.println(digitalRead(reedSwitch2));
  Serial.println("delaying...");
  for (int i = 0; i < sleepcycles; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);    //sleep 8 seconds <-- DOESN'T WORK!
  }
  //sleep.pwrDownMode(); //set sleep mode
  //sleep.sleepDelay(sleepTime); //sleep for: sleepTime
  //delay(5000);
}


/*
/* DECODER
  function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.

  var decoded = {};
 
  if(bytes.length == 6){ //BME280_lora.ino
    decoded.degreesC = (bytes[0]<<24>>16 | bytes[1]) /100;
    decoded.pressure = ((bytes[2] << 24) + (bytes[3] << 16) + (bytes[4] << 8) + (bytes[5])) / 100;
  } 
  else if(bytes.length == 4) { //door_lora.ino
    decoded.switch1 = (bytes[0]);
    decoded.switch2 = (bytes[1]);
    decoded.vbat = (bytes[2]<<24>>16 | bytes[3]) /100;
  } else {
  decoded.degreesC = (bytes[0]<<24>>16 | bytes[1]) /100; //bmp280_lora
  decoded.humidity = (bytes[2]<<24>>16 | bytes[3]) /100;
  decoded.pressure = ((bytes[4] << 24) + (bytes[5] << 16) + (bytes[6] << 8) + (bytes[7])) / 100;
  decoded.vbat = (bytes[8]<<24>>16 | bytes[9]) /100;
  }
  return decoded;
  }
*/
