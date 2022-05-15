// Capteur LoRa Intérieur Maison
/* CONFIDENTIAL */

/************************** Configuration ***********************************/
const unsigned int sendInterval = 5;
#include "secrets.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <TinyLoRa.h>
#include <SPI.h>
#include <Sleep_n0m1.h>

Sleep sleep;
unsigned long sleepTime = 300000;
Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

#define VBATPIN A9

// Data Packet to Send to TTN
//unsigned char loraData[10];
unsigned char loraData[6];

// Pinout for Adafruit Feather 32u4 LoRa
TinyLoRa lora = TinyLoRa(7, 8, 4);

void setup()
{
  Serial.begin(9600);
  unsigned status;
  // Initialize pin LED_BUILTIN as an output
  pinMode(LED_BUILTIN, OUTPUT);
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
  Serial.println("OK");
  //while(!Serial);
  while(!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

}

void loop()
{
  sensors_event_t temp_event, pressure_event;
  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float temperature = temp_event.temperature;
  // Read Pressure
  float pressure = pressure_event.pressure;


  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(pressure)) {
    Serial.println("Failed to read sensor!");
    return;
  }

  // encode float as int
  int16_t tempInt = temperature * 100;
  long pressInt = pressure * 100;
  //float measuredvbat = analogRead(VBATPIN);
  //measuredvbat *= 2;    // we divided by 2, so multiply back
  //measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  //measuredvbat /= 1024; // convert to voltage

  // Negative Values : https://www.thethingsnetwork.org/forum/t/negative-temperature/21330/3
  // Encoding to bytes :  https://www.thethingsnetwork.org/docs/devices/bytes.html

  // encode int as bytes
  //byte payload[2];
  loraData[0] = highByte(tempInt);
  loraData[1] = lowByte(tempInt);

  loraData[2] = (byte) ((pressInt & 0xFF000000) >> 24 );
  loraData[3] = (byte) ((pressInt & 0x00FF0000) >> 16 );
  loraData[4] = (byte) ((pressInt & 0x0000FF00) >> 8  );
  loraData[5] = (byte) ((pressInt & 0X000000FF)       );

  //loraData[8] = highByte(measuredvbat);
  //loraData[9] = lowByte(measurevbat);

  Serial.println("Sending LoRa Data...");
  lora.sendData(loraData, sizeof(loraData), lora.frameCounter);
  Serial.print("Frame Counter: "); Serial.println(lora.frameCounter);
  lora.frameCounter++;

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");

  Serial.print(pressure);
  Serial.println(" hPa");

  //Serial.print("VBat: " );
  //Serial.println(measuredvbat);

  Serial.println();

  // blink LED to indicate packet sent
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("delaying...");
  //delay(1000);
  //delay(sendInterval * 1000 * 60);
  
  sleep.pwrDownMode(); //set sleep mode
  sleep.sleepDelay(sleepTime); //sleep for: sleepTime
}

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
