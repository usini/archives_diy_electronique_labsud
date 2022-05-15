
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_MAX7219_32X8_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ D5, /* data=*/ D6, /* cs=*/ D7, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);


uint16_t pos;
uint16_t len;
char *str = "    Arduino Days 2019    ";

void setup(void) {
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_victoriabold8_8r);  // choose a 8x8 u8g2 font
  
  pos = 0;
  len = strlen(str)*8;
}

#define BUF_LEN 12
void draw_string_at_bitpos(const char *str, uint16_t bitpos)
{
  char buf[BUF_LEN];
  uint16_t start = bitpos / 8;
  uint16_t i;
  for( i = 0; i < BUF_LEN-1; i++)
  {
    buf[i] = str[start+i];
    if ( str[start+i] == '\0' )
      break;
  }
  buf[BUF_LEN-1] = '\0';
  u8g2.drawStr( -(bitpos & 7), 8, buf);     // write buffer  
}

void loop(void) {
  u8g2.clearBuffer();         // clear the internal memory
  draw_string_at_bitpos(str, pos);
  u8g2.sendBuffer();          // transfer internal memory to the display
  pos++;
  if ( pos >= len )
    pos = 0;
  delay(50);  
}
