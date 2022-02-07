#include "lin_bus.h"
#include "ncv7430.h"

// Create an IntervalTimer object 
IntervalTimer myTimer;

int ledState = LOW;                // ledState used to set the LED
unsigned long interval = 200000;   // interval at which to blinkLED to run every 0.2 seconds

NCV7430 ncv7430(&Serial1);

int lin_cs = 23;
int intensity = 3;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(lin_cs, OUTPUT);
  digitalWrite(lin_cs, HIGH);
  
  Serial.begin(115200);
  Serial.println("NVC7430 Reset OTP");
  
  ncv7430.init();
  myTimer.begin(blinkLED, interval);
}

void loop() {
  ncv7430.set_otp_param(0x04, 0);
  ncv7430.set_otp_param(0x05, 0);
  ncv7430.set_otp_param(0x06, 0);
  ncv7430.set_otp_param(0x07, 0);
  ncv7430.set_otp_param(0x08, 0);
  ncv7430.set_otp_param(0x0A, 0);
  ncv7430.set_otp_param(0x0B, 0);
  ncv7430.set_otp_param(0x0C, 0);
  ncv7430.set_otp_param(0x0D, 0);
  // a power-on reset is needed to set the parameter, so re-plug it set the new values
  // the lockbits and command off are prevent from setting with good intent
  ncv7430.sleep();
}

void blinkLED() {
  ledState = !ledState;
  
  digitalWrite(LED_BUILTIN, ledState);
}