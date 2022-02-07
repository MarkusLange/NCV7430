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
  Serial.println("NVC7430 RGB Demo");
  
  ncv7430.init();
  myTimer.begin(blinkLED, interval);
}

void loop() {
  // Red
  Serial.println("red");
  ncv7430.set_color(intensity, rgb_r);
  delay(1000);
  
  // Green
  Serial.println("green");
  ncv7430.set_color(intensity, rgb_g);
  delay(1000);
  
  // Blue
  Serial.println("blue");
  ncv7430.set_color(intensity, rgb_b);
  delay(1000);
}

void blinkLED() {
  ledState = !ledState;
  
  digitalWrite(LED_BUILTIN, ledState);
}