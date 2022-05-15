/* SETTINGS */
#include <Wire.h> //I2C
#include <U8g2lib.h>
U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, SCL, SDA);   // pin remapping with ESP8266 HW I2C
String oled_line1;
String oled_line2;

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  u8g2.enableUTF8Print();
  oled_line1 = "Salut Labsud";
  oled_line2 = "Rémi Sarrailh";
  printScreen();
}

void loop()
{
    //printScreen();
}

void printScreen() {
  u8g2.setFont(u8g2_font_unifont_t_latin);
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 14);
    u8g2.println(oled_line1);  // write something to the internal memory
    u8g2.setCursor(0, 30);
    u8g2.println(oled_line2);  // write something to the internal memory
  }
  while ( u8g2.nextPage() );
}
