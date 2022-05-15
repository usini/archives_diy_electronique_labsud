#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

void getWeather(double &temperature ,double &humidity, double &pressure) {
  if (bme.begin(0x76, &Wire)) {
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100;
  } else {
    temperature = NAN;
    humidity = NAN;
    pressure = NAN;
  }
}
