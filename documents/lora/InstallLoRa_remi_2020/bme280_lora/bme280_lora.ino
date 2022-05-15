#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <TinyLoRa.h>
#include "secrets.h"
#include <Sleep_n0m1.h>
Adafruit_BME280 bme; // I2C
Sleep sleep;
unsigned long sleepTime = 300000;
#define VBATPIN A9

// Visit your thethingsnetwork.org device console
// to create an account, or if you need your session keys.

/************************** Example Begins Here ***********************************/
// Pinout for Adafruit Feather M0 LoRa
//TinyLoRa lora = TinyLoRa(3, 8);

// Data Packet to Send to TTN
unsigned char loraData[10];

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

  status = bme.begin(BME280_ADDRESS_ALTERNATE);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1);
  }
}

void loop()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity = bme.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = bme.readTemperature();
  // Read Pressure
  float pressure = bme.readPressure() / 100.0F;


  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature) || isnan(pressure)) {
    Serial.println("Failed to read sensor!");
    return;
  }

  // encode float as int
  int16_t tempInt = temperature * 100;
  int16_t humidInt = humidity * 100;
  long pressInt = pressure * 100;
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  int16_t vbat = measuredvbat * 100;

  // Negative Values : https://www.thethingsnetwork.org/forum/t/negative-temperature/21330/3
  // Encoding to bytes :  https://www.thethingsnetwork.org/docs/devices/bytes.html

  // encode int as bytes
  //byte payload[2];
  loraData[0] = highByte(tempInt);
  loraData[1] = lowByte(tempInt);

  loraData[2] = highByte(humidInt);
  loraData[3] = lowByte(humidInt);

  loraData[4] = (byte) ((pressInt & 0xFF000000) >> 24 );
  loraData[5] = (byte) ((pressInt & 0x00FF0000) >> 16 );
  loraData[6] = (byte) ((pressInt & 0x0000FF00) >> 8  );
  loraData[7] = (byte) ((pressInt & 0X000000FF)       );

  loraData[8] = highByte(vbat);
  loraData[9] = lowByte(vbat);

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

  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("VBat: " );
  Serial.println(measuredvbat);

  Serial.println();

  // blink LED to indicate packet sent
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("delaying...");
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
