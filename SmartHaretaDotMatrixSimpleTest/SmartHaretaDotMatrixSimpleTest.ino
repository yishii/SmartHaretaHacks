/**
 * SmartHareta Dot-matrix LED simple demo program v0.01
 */

#include <Wire.h>

#define FB_SIZE_X 24
#define FB_SIZE_Y 24
static uint8_t framebuffer[FB_SIZE_X * FB_SIZE_Y / 8];

void bitBlt(const uint8_t* pix, int ox, int oy, size_t sx, size_t sy)
{
  int x, y;
  for (y = 0; y < sy; y++) {
    for (x = 0; x < sx; x++) {
      if (pix[(y * (sx / 8)) + (x / 8)] & (1 << (7 - (x % 8)))) {
      } else {
        setPoint(ox + x, oy + y);
      }
    }
  }
}

void setPoint(int x, int y)
{
  if ((x < 0) || (y < 0) || (x >= FB_SIZE_X) || (y >= FB_SIZE_Y)) {
    return;
  }

  // Process as x and y switched
  framebuffer[x * 3 + (2 - (y / 8))] |= 1 << (y % 8);
}

void unsetPoint(int x, int y)
{
  if ((x < 0) || (y < 0) || (x >= FB_SIZE_X) || (y >= FB_SIZE_Y)) {
    return;
  }

  // Process as x and y switched
  framebuffer[x * 3 + y / 8] &= ~(1 << (y % 8));
}

void clear(int color)
{
  memset(framebuffer, color ? 0xff : 0x00, sizeof(framebuffer));
}

static void updateDriver(uint8_t addr, uint8_t* b)
{
  Wire.beginTransmission(addr);
  Wire.write(0x00);
  for (int i = 0; i < 8; i++) {
    Wire.write(*(b + i));
    Wire.write(*(b + i + 8));
  }
  Wire.endTransmission();
}

void updateLed(void)
{
  int y;
  uint8_t buff[16];

  for (y = 0; y < 16; y++) {
    buff[y] = framebuffer[y * 3];
  }
  updateDriver(0x72, buff);

  for (y = 0; y < 16; y++) {
    buff[y] = framebuffer[y * 3 + 1];
  }
  updateDriver(0x71, buff);

  for (y = 0; y < 16; y++) {
    buff[y] = framebuffer[y * 3 + 2];
  }
  updateDriver(0x70, buff);

  for (y = 0; y < 8; y++) {
    buff[y] = framebuffer[3 * 16 + y * 3 + 1];
    buff[y + 8] = framebuffer[3 * 16 + y * 3 + 1 + 1];
  }
  updateDriver(0x73, buff);

  for (y = 0; y < 8; y++) {
    buff[y] = framebuffer[3 * 16 + y * 3];
  }
  updateDriver(0x74, buff);
}

void setupDriver(void)
{
  uint8_t a;
  for (a = 0x70; a <= 0x74; a++) {
    initializeLedDriver(a);
  }
}

static void initializeLedDriver(uint8_t addr)
{
  /**
     System Setup Register : Turn on system oscillator
  */
  Wire.beginTransmission(addr);
  Wire.write(0x21);
  Wire.endTransmission();

  /**
     Display Setup Register
  */
  Wire.beginTransmission(addr);
  Wire.write(0x81);
  Wire.endTransmission();

  /**
     Digital Dimming Data Input (LED Duty configuration)
  */
  {
    uint8_t brightness = 10; /* 0 - 15 */
    Wire.beginTransmission(addr);
    Wire.write(0xe0 | brightness);
    Wire.endTransmission();
  }
}

/**************************************************************************/

void setup()
{
  int i;

  Wire.begin(25, 26);
  Wire.setClock(200000L);
  
  setupDriver();
  
  clear(0);

  for(i=0; i<23; i++){
    setPoint(i, 0);
    setPoint(i, 23);
    setPoint(0, i);
    setPoint(23, i);
  }
  
  updateLed();
}

void loop() {  
}
