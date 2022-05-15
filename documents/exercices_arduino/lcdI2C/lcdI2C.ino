#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  lcd.init(); // Initialise le lcd
  lcd.backlight();
  lcd.clear();
  
              //x  y
  lcd.setCursor(0, 0);
  lcd.print("Salut");
  lcd.setCursor(0, 1);
  lcd.print("le labsud");
}

void loop() {

}
