#include "lin_bus.h"
#include "ncv7430.h"

// Create an IntervalTimer object 
IntervalTimer myTimer;

int ledState = LOW;                // ledState used to set the LED
unsigned long interval = 200000;   // interval at which to blinkLED to run every 0.2 seconds

NCV7430 ncv7430(&Serial1);

int lin_cs = 23;

//                  R    G    B
uint8_t rgb_r[3] = {255,   0,   0};
uint8_t rgb_g[3] = {  0, 255,   0};
uint8_t rgb_b[3] = {  0,   0, 255};
int intensity = 3;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(lin_cs, OUTPUT);
  digitalWrite(lin_cs, HIGH);
  
  Serial.begin(115200);
  Serial.println("NVC7430 Fade Demo");
  
  ncv7430.init();
  myTimer.begin(blinkLED, interval);
}

void loop() {
  Serial.println("color fade");
  float cfade_delay = 4.0;
  ncv7430.set_color_fade(intensity, cfade_delay, rgb_r, rgb_b);
  delay((int)cfade_delay*1000);
  
  Serial.println("intensity fade");
  float ifade_delay = 6.3;
  ncv7430.set_intensity_fade(0, 7, ifade_delay, rgb_g);
  delay((int)ifade_delay*1000);
  ncv7430.set_intensity_fade(7, 0, ifade_delay, rgb_g);
  delay((int)ifade_delay*1000);
}

void blinkLED() {
  ledState = !ledState;
  
  digitalWrite(LED_BUILTIN, ledState);
}