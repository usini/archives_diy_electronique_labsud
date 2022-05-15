/* ESP32 Weather Station By Remi Sarrailh
  Licence : MIT
  Twitter @m4dnerd
*/

/*
Settings ---
To use global settings create in libraries : arduino_secrets/arduino_secrets.h
This settings will be available in all your sketches
*/
//#include <arduino_secrets.h> //Settings (global)
#include "arduino_secrets.h" //Settings (local)

/* I2C */
const int PIN_SDA = 19;
const int PIN_SCL = 23;
const int PIN_PIR = 17; // PIR
const int SERIAL_BAUD = 115200; //Serial
const uint64_t SLEEPFOR = 5 * 60000000;

/* INTERNET */
const char* NAME = "meteo";
#define SSID1 HOME_SSID
#define PASS1 HOME_PASS
#define SSID2 ALT_SSID
#define PASS2 ALT_PASS

/* MQTT */
const int   MQTT_PORT  = ADAFRUIT_MQTT_PORT;
const char* MQTT_SERVER = ADAFRUIT_MQTT_SERVER;
const char* MQTT_USERNAME = ADAFRUIT_MQTT_USERNAME;
const char* MQTT_PASS      = ADAFRUIT_MQTT_PASS;
const char* CERT = ADAFRUIT_CERT;
const char* TEMP_NAME  = "madnerd/feeds/station-meteo.temperature";
const char* BARO_NAME  = "madnerd/feeds/station-meteo.pression-atmospherique";
const char* LIGHT_NAME = "madnerd/feeds/station-meteo.luminosite";
const char* HUM_NAME   = "madnerd/feeds/station-meteo.humidite";
const char* PIR_NAME = "madnerd/feeds/station-meteo.presence";

/* Discord */
const String DISCORD_TTS = "true";

#include <Wire.h>
#include "oledi2c_128_32.h" //Display
#include "tsl2561.h" //Light
#include "bme280.h" //Weather
#include "button.h" //Button
#include "sleep.h"
#include "pir.h" //PIR
#include "mqtt.h" //MQTT
#include "wifi.h" //Wifi
#include "discord.h" //Discord

//Topic publish
Adafruit_MQTT_Publish temp_out = Adafruit_MQTT_Publish(&mqtt, TEMP_NAME);
Adafruit_MQTT_Publish baro_out = Adafruit_MQTT_Publish(&mqtt, BARO_NAME);
Adafruit_MQTT_Publish hum_out = Adafruit_MQTT_Publish(&mqtt, HUM_NAME);
Adafruit_MQTT_Publish light_out = Adafruit_MQTT_Publish(&mqtt, LIGHT_NAME);
Adafruit_MQTT_Publish pir_out = Adafruit_MQTT_Publish(&mqtt, PIR_NAME);
double temperature = NAN, humidity = NAN, pressure = NAN;
float light = NAN;
bool pirState = false;

void setup() {
  Serial.begin(SERIAL_BAUD);

  //pinMode(15, OUTPUT);
  //digitalWrite(15, HIGH);
  Wire.begin(PIN_SDA, PIN_SCL);
  getWeather(temperature, humidity, pressure);
  getLux(light);
  getPIR(pirState);

  startDisplay();
  displayData();
  serialData();

  //https://lastminuteengineers.com/esp32-deep-sleep-wakeup-sources/
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  startWifi();
  //Serial.println(wakeup_reason);
  if (wakeup_reason == 2) { //Wake up from PIR
    //sendDiscord(WEBHOOK_PIR, "Présence detecté");
    //sendDiscord(WEBHOOK_WEATHER, "la température est de " + String(temperature) + "°. L'humidité est de " + String(humidity) + "% ." + "La pression atmosphérique est de " + String(int(pressure)) + " hecto Pascals. " + "La luminosité est de " + String(int(light)) + " lux.");
    sendMQTT(temperature, humidity, pressure, light, 1);
  } else { //Wake up from sleep
    //sendDiscord(WEBHOOK_WEATHER, "la température est de " + String(temperature) + "°. L'humidité est de " + String(humidity) + "% ." + "La pression atmosphérique est de " + String(int(pressure)) + " hecto Pascals. " + "La luminosité est de " + String(int(light)) + " lux.");
    sendMQTT(temperature, humidity, pressure, light, 0);
  }
  stopDisplay();

  sleepMode();
}

void loop() {
  //Nothing to do here
}

void sendMQTT(double &temperature, double &humidity, double &pressure, float &light, int pirState){
    verifyFingerprint();
    while(!mqtt_ok){
        Serial.println("[MQTT] Sending MQTT...");
        MQTT_connect();
        mqtt_ok = temp_out.publish(temperature);
        mqtt_ok = baro_out.publish(pressure);
        mqtt_ok = hum_out.publish(humidity);
        mqtt_ok = light_out.publish(light);
        mqtt_ok = pir_out.publish(pirState);
    }
    Serial.println("[MQTT] Sent...");
}

void serialData() {
  Serial.println("[SERIAL] " + String(temperature) + "°C");
  Serial.println("[SERIAL] " + String(humidity) + "%");
  Serial.println("[SERIAL] " + String(pressure) + " hPa");
  Serial.println("[SERIAL] " + String(light) + " Lux");
  Serial.println("[SERIAL] " + String(pirState));
}

void displayData() {
  String oled_line1 = "";
  String oled_line2 = "";
  oled_line1 = String(temperature) + "°C" + " ";
  oled_line1 = oled_line1 + String(int(pressure)) + "hPa";
  oled_line2 = oled_line2 + String(humidity) + "%" + " ";
  oled_line2 = oled_line2 + String(int(light)) + "lux";
  printDisplay(oled_line1, oled_line2);
}
