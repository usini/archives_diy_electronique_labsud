#define mario_w 16
#define mario_h 16
static const unsigned char mario_idle[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x01, 0xE0, 0x0F, 
  0x70, 0x02, 0xD8, 0x00, 0x18, 0x06, 0x00, 0x00, 0xB0, 0x05, 0xB8, 0x0D, 
  0xB8, 0x1D, 0xF8, 0x1F, 0x60, 0x06, 0x70, 0x0E,
};

static const unsigned char mario_walk1[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0xC0, 0x1F, 0xE0, 0x04,
  0xB0, 0x01, 0x30, 0x0C, 0x00, 0x00, 0x70, 0x03, 0x7C, 0x3F, 0x7C, 0x3F,
  0xF0, 0x13, 0xF8, 0x1F, 0x38, 0x1E, 0x70, 0x00,
};

static const unsigned char mario_walk2[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x01, 0xE0, 0x0F, 0x70, 0x02,
  0xD8, 0x00, 0x18, 0x06, 0x00, 0x00, 0x50, 0x01, 0x58, 0x03, 0x58, 0x05,
  0xF8, 0x07, 0xF0, 0x03, 0xE0, 0x07, 0xE0, 0x01,
};

static const unsigned char mario_jump1[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0xC0, 0x1F,
  0xE0, 0x04, 0xB0, 0x01, 0x30, 0x0C, 0x00, 0x00, 0xE0, 0x35, 0xE0, 0x3F,
  0xE0, 0x0F, 0xF0, 0x07, 0x78, 0x03, 0x08, 0x07,
};

static const unsigned char mario_jump2[] U8X8_PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x01, 0xE0, 0x3F,
  0x70, 0x32, 0xD8, 0x30, 0x18, 0x1E, 0x00, 0x08, 0xF8, 0x07, 0xFC, 0x27,
  0x70, 0x3B, 0xF8, 0x3F, 0xFC, 0x03, 0x64, 0x00,
};
