//MQTT
#include <WiFiClientSecure.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

bool mqtt_ok = false;

WiFiClientSecure clientMQTT;
Adafruit_MQTT_Client mqtt(&clientMQTT, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASS);

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
    int8_t ret;

    // Stop if already connected.
    //if (mqtt.connected()) {
        //Serial.println("[MQTT] Connected!");
        //return;
    //}

    Serial.print("[MQTT] Connecting... ");

    uint8_t retries = 2;
    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println("[MQTT] Retrying connection in 10 ms...");
        mqtt.disconnect();
        delay(10);  // wait 5 seconds
        retries--;
        if (retries == 0) {
            // basically die and wait for WDT to reset me
            //while (1);
            Serial.println("[MQTT] Connection Failed");
        }
    }
    Serial.println("[MQTT] Connected!");
}

void verifyFingerprint() {

  const char* host = MQTT_SERVER;

  Serial.print("[MQTT] Checking Host identity: ");
  Serial.print(host);
  Serial.print(":");
  Serial.println(MQTT_PORT);


  clientMQTT.setCACert(CERT);

  if (! clientMQTT.connect(host, MQTT_PORT)) {
    Serial.println("[MQTT] Host verification failed. Halting execution.");
    char err[1024];
    clientMQTT.lastError(err, 1024);
    Serial.println(err);
    delay(1000);
  }

  Serial.println("[MQTT] Host identity checked... ");
}




