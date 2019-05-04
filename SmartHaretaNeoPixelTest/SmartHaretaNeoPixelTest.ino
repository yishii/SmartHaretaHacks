#include <FastLED.h>

#define NUM_LEDS 17
#define DATA_PIN 27

CRGB leds[NUM_LEDS];

void setup()
{
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop(void)
{
  static int cnt;
  int i;

  for (i = 0; i < 17; i++) {
    leds[i] = CRGB(
                (cnt & 4) ? 0x30 : 0x00,
                (cnt & 2) ? 0x30 : 0x00,
                (cnt & 1) ? 0x30 : 0x00);
  }
  FastLED.show();
  cnt++;
  delay(1000);
}
