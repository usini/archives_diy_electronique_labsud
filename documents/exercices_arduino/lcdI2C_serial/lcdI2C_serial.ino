#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <elapsedMillis.h>

const String usb_name = "lcd16x2";
LiquidCrystal_I2C lcd(0x27,16,2); 


elapsedMillis timeElapsed; //declare global if you don't want it reset every time loop runs
unsigned int interval = 5000;

//Serial string buffer
String readString;

// Serial

//When application asked if this is the correct arduino
void serialCheck(){
    if (readString == "/info") {
      Serial.println(usb_name);
   }
}

//Convert characters sent by serial to string
void serialManager(){
    //Get Serial as a string
  while (Serial.available()) {
    delay(3); // Wait for data

    //Convert to String
    if (Serial.available() > 0) {
      char c = Serial.read();
      readString += c;
    }
  }
}

void setup() {
  //Setup Serial
  Serial.begin(115200);
  lcd.init(); // setup LCD
  lcd.backlight();
  
}

void loop() {
  serialManager();

  //If string received
     if (readString.length() > 0) {
      lcd.backlight();
      serialCheck();
       lcd.clear();
       lcd.setCursor(0,0);
       int stringSize = readString.length();
       if (stringSize > 16){
         String line1 = readString.substring(0,16);
         Serial.println("l1:" + line1);
         lcd.print(line1);
         String line2 = readString.substring(16,stringSize);
         Serial.println("l2:" + line2);
         lcd.setCursor(0,1);
         lcd.print(line2);
         
       } else
       {
        lcd.print(readString);
        Serial.println("l1:"+readString);
       }
       timeElapsed = 0;
       
    } else {
      if(timeElapsed > interval){
        lcd.noBacklight();
        timeElapsed = 0;
        Serial.println("BACKLIGHT:OFF");
      }
    }
     
   

  //We clean the serial buffer
  readString = "";
}
