#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>
WiFiMulti WiFiMulti;

void startWifi() {
  WiFiMulti.addAP(SSID1, PASS1);
  WiFiMulti.addAP(SSID2,PASS2);
  WiFi.mode(WIFI_STA);
  Serial.println("[WiFi] Connecting...");
  // wait for WiFi connection
  //Todo get reading pir when connection is in progress
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println("[WiFi] Connected");
  MDNS.begin(NAME);
}

void stopWifi(){
  WiFi.mode(WIFI_OFF);
}
