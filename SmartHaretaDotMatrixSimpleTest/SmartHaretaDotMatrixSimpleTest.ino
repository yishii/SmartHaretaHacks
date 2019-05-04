/**
 * SmartHareta Dot-matrix LED simple demo program v0.01
 */

#include <Wire.h>

void setupDriver(void)
{
  uint8_t a;
  for (a = 0x70; a <= 0x74; a++) {
    initializeLedDriver(a);
  }
}

void initializeLedDriver(uint8_t addr)
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

void sendToDriver(uint8_t addr)
{
  Wire.beginTransmission(addr);
  Wire.write(0x00);
  for (int i = 0; i < 8; i++) {
    if(i & 1){
      Wire.write(0xff);
      Wire.write(0xaa);
    } else {
      Wire.write(0x00);
      Wire.write(0x55);
    }
  }
  Wire.endTransmission();
}

void setup()
{
  uint8_t a;

  Wire.begin(25, 26);
  Wire.setClock(200000L);
  
  setupDriver();

  for (a = 0x70; a <= 0x74; a++) {
    sendToDriver(a);
  }
}

void loop() {  
}
