#include "FastLED.h"

// Pride2015
// Animated, ever-changing rainbows.
// by Mark Kriegsman

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN 3
//#define CLK_PIN   4
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 200
#define BRIGHTNESS 255

CRGB leds[NUM_LEDS];

int input1 = 5;  // 定义uno的pin 5 向 input1 输出
int input2 = 6;  // 定义uno的pin 6 向 input2 输出
int enA = 10;    // 定义uno的pin 10 向 输出A使能端输出

void setup() {
  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(A0, INPUT); 
  pinMode(A1, INPUT);
  digitalWrite(input1, LOW);  //给低电平
  digitalWrite(input2, LOW);  //给低电平
  delay(3000);  // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  int v = analogRead(A0);
  int res = analogRead(A1);
  Serial.println(v);
  Serial.println(res);

  analogWrite(enA, v / 4);

  if (res < 500) { 
    digitalWrite(input1, HIGH);  //给高电平
    digitalWrite(input2, LOW);   //给低电平
    pride();
    FastLED.show();
  } else {
    digitalWrite(input1, LOW);  //给低电平
    digitalWrite(input2, LOW);  //给低电平
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
      leds[i]=CRGB::Black;
    }
    FastLED.show();
  }
}

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride() {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;  //gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV(hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 64);
  }
}
