/**
 * SmartHareta Dot-matrix LED demo program v0.01
 */

#include <Wire.h>

static uint8_t framebuffer[24 * 24 / 8];
#define FB_SIZE_X 24
#define FB_SIZE_Y 24

static uint8_t characters[4][32] =
{
  {
    0xff, 0xff, 0xfc, 0x1f, 0xe3, 0xe3, 0xdf, 0xfb, 0xdf, 0xfd, 0xbf, 0xad,
    0x7f, 0xae, 0x7f, 0xae, 0x7e, 0xfa, 0x7f, 0xfe, 0xb7, 0xda, 0xc7, 0xf9,
    0xe3, 0xf3, 0xdc, 0x0d, 0xbe, 0x3e, 0xc1, 0x81
  },
  {
    0xff, 0xff, 0xf8, 0x3f, 0xe7, 0xcf, 0xdf, 0xf7, 0xdf, 0xfb, 0xbf, 0xeb,
    0xbd, 0xeb, 0xbe, 0xab, 0xbe, 0xfb, 0xbd, 0xeb, 0x03, 0xfb, 0x6f, 0xf7,
    0x77, 0xe3, 0x70, 0x1d, 0x8f, 0xbd, 0xff, 0xc3
  },
  {
    0xff, 0xff, 0xfe, 0x3f, 0xf1, 0xc7, 0xcf, 0xf7, 0xbf, 0xfb, 0xbf, 0xa9,
    0x7f, 0xaa, 0x7f, 0xaa, 0x6e, 0xfa, 0x6f, 0xda, 0x8f, 0xf9, 0xc7, 0xf7,
    0xd8, 0x0f, 0xef, 0xaf, 0xf0, 0x5f, 0xff, 0xff
  },
  {
    0xff, 0xff, 0xf8, 0x1f, 0xe7, 0xef, 0xdf, 0xf1, 0x87, 0xfa, 0x7f, 0x5a,
    0x7f, 0x5a, 0x7f, 0x5a, 0x7d, 0xf1, 0xbf, 0xbb, 0x3f, 0xf5, 0x5f, 0xf5,
    0x63, 0x8d, 0xb8, 0x3b, 0xdd, 0xc7, 0xe3, 0xff
  }
};

void bitBlt(uint8_t* pix, int ox, int oy, size_t sx, size_t sy)
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

#if 1 // for invert image(trial)
  for (y = 0; y < sizeof(framebuffer); y++) {
    framebuffer[y] = ~framebuffer[y];
  }
#endif

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

void setup()
{
  Wire.begin(25, 26);
  Wire.setClock(200000L);

  setupDriver();

  clear(0);
  updateLed();
}

void draw_floor(int c)
{
  int x;
  int s = 0;
  bool f;
  int y;

  y = 22;

  for (x = 0; x < 24; x++) {
    if (((c + s) % 4) == 0) {
      y = 20;
    } else {
      y = 21;
    }
    s++;
    setPoint(x, y);
  }
}

void demo(void)
{
  int32_t counter;
  const int motion[] = { 1, 2, 3, 2 };
  int motion_no = 0;
  int current_motion = 0;

  counter = 0;

  clear(0);
  draw_floor(counter);
  bitBlt(characters[0], 4, 4, 16, 16);
  delay(1000);

  while (1) {
    clear(0);
    draw_floor(counter / 4);
    if ((counter % 10) == 0) {
      current_motion = motion_no % (sizeof(motion) / sizeof(motion[0]));
      motion_no++;
    }
    bitBlt(characters[motion[current_motion]], 4, 4, 16, 16);
    updateLed();
    delay(16);
    counter++;
  }
}

void loop() {
  clear(0);
  updateLed();

  demo();
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
