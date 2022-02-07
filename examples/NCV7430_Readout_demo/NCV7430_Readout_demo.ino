#include "lin_bus.h"
#include "ncv7430.h"

// Create an IntervalTimer object 
IntervalTimer myTimer;

int ledState = LOW;                // ledState used to set the LED
unsigned long interval = 200000;   // interval at which to blinkLED to run every 0.2 seconds

NCV7430 ncv7430(&Serial1);

#define len 8
uint8_t lin_frame[len];
uint8_t otp_param1[len];
uint8_t otp_param2[len];
uint8_t full_status[len];
uint8_t act_param1[len];
uint8_t act_param2[len];
uint8_t color[len];
uint8_t status[2];
uint8_t led_control[2];

int lin_cs = 23;
int intensity = 3;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(lin_cs, OUTPUT);
  digitalWrite(lin_cs, HIGH);
  
  Serial.begin(115200);
  Serial.println("NVC7430 Readout Demo");
  
  ncv7430.init();
  myTimer.begin(blinkLED, interval);
}

void loop() {
  Serial.println("-------------------");
  opt_parameter1();
  Serial.println("-------------------");
  opt_parameter2();
  Serial.println("-------------------");
  get_full_status();
  Serial.println("-------------------");
  get_status();
  Serial.println("-------------------");
  get_color();
  Serial.println("-------------------");
  get_led_control();
  Serial.println("-------------------");
  get_act_param1();
  Serial.println("-------------------");
  get_act_param2();
  Serial.println("-------------------");
  delay(3000);
}

void opt_parameter1(){
  Serial.println("Get OTP1 Parameter");
  ncv7430.get_otp_param1(otp_param1);
  frame(otp_param1, 8);
  Serial.print("AD: ");
  Serial.println(otp_param1[0]&0x3F,HEX);
  // Based on NCV7430GEVB EVAL BOARD USER’S MANUAL, Page 8 Figure 13 its
  // Ballast whats stored in Diagalloff
  Serial.print("Ballast:  ");
  Serial.println(otp_param1[1]&0x01);
  
  Serial.print("LOW BAUD: ");
  if (otp_param1[1]&0x02)
    Serial.println("10400 BAUD");
  else
    Serial.println(" 9600 BAUD");
  
  // otp_param1[2] = 0xFF
  Serial.print("TWPROG:  ");
  Serial.println((otp_param1[3]&0x20) >> 5);
  
  Serial.print("ERROFF:  ");
  Serial.println((otp_param1[3]&0x40) >> 6);
  
  Serial.print("LEDS Default on: ");
  Serial.println((otp_param1[3]&0x80) >> 7);
  
  Serial.print("AD: ");
  Serial.println(otp_param1[4]&0x3F,HEX);
  
  Serial.print("CMDSOFF: ");
  Serial.println((otp_param1[4]&0x40) >> 6);
  
  Serial.print("LOCKBT1: ");
  Serial.println((otp_param1[4]&0x80) >> 7);
  
  Serial.print("LED a11: ");
  Serial.println(otp_param1[5],HEX);
  
  Serial.print("LED a12: ");
  Serial.println(otp_param1[6],HEX);
  
  Serial.print("LED a13: ");
  Serial.println(otp_param1[7],HEX);
}

void opt_parameter2(){
  Serial.println("Get OTP2 Parameter");
  ncv7430.get_otp_param2(otp_param2);
  frame(otp_param2, 8);
  
  Serial.print("AD: ");
  Serial.println(otp_param2[0]&0x3F,HEX);
  
  Serial.print("LED a21: ");
  Serial.println(otp_param2[1],HEX);
  
  Serial.print("LED a22: ");
  Serial.println(otp_param2[2],HEX);
  
  Serial.print("LED a23: ");
  Serial.println(otp_param2[3],HEX);
  
  Serial.print("LED a31: ");
  Serial.println(otp_param2[4],HEX);
  
  Serial.print("LED a32: ");
  Serial.println(otp_param2[5],HEX);
  
  Serial.print("LED a33: ");
  Serial.println(otp_param2[6],HEX);
  
  Serial.print("GROUP_ID:     ");
  Serial.println(otp_param2[7]&0x0F);
  
  Serial.print("TH_CONT:      ");
  Serial.println((otp_param2[7]&0x10) >> 4);
  
  // otp_param2[7]&0x20 = 1
  Serial.print("LED_MOD_FREQ: ");
  Serial.println((otp_param2[7]&0x40) >> 6);
  
  Serial.print("LOCKBT2:      ");
  Serial.println((otp_param2[7]&0x80) >> 7);
}

void get_full_status(){
  Serial.println("Get Full Status");
  ncv7430.get_full_status(full_status);
  frame(full_status, 8);
  Serial.print("AD: ");
  Serial.println(full_status[0]&0x3F,HEX);

  // full_status[1] = 0xFF
  Serial.print("LIN Bit Error:    ");
  Serial.println(full_status[2]&0x01);

  Serial.print("LIN Header Error: ");
  Serial.println((full_status[2]&0x02) >> 1);
  
  Serial.print("LIN Data Error:   ");
  Serial.println((full_status[2]&0x04) >> 2);
  
  // 00:         T < T<TW>
  // 01: T<TW> < T < T<TSD>
  // 11:         T > T<TSD>
  Serial.print("Tinfo:       ");
  Serial.println(full_status[4]&0x03);
  
  Serial.print("TW:          ");
  Serial.println((full_status[3]&0x04) >> 2);
  
  Serial.print("TSD:         ");
  Serial.println((full_status[3]&0x08) >> 3);
  
  Serial.print("VBB Reset:   ");
  Serial.println((full_status[3]&0x10) >> 4);
  
  Serial.print("VBB Under Voltage: ");
  Serial.println((full_status[3]&0x20) >> 5);
  
  Serial.print("Error LED1:  ");
  Serial.println(full_status[4]&0x07);
  
  Serial.print("RETRY state: ");
  Serial.println((full_status[4]&0x08) >> 3);
  
  Serial.print("Thermal Control Status: ");
  Serial.println((full_status[4]&0xF0) >> 4);
  
  Serial.print("Error LED2:  ");
  Serial.println(full_status[5]&0x03);
  
  Serial.print("Error LED3:  ");
  Serial.println((full_status[5]&0x70) >> 4);
  
  Serial.print("GROUP_ID:    ");
  Serial.println(full_status[6]&0x0F);
  
  Serial.print("LEDs on/off: ");
  Serial.println((full_status[6]&0x10) >> 4);
  
  Serial.print("LED1 enable: ");
  Serial.println((full_status[6]&0x20) >> 5);
  
  Serial.print("LED2 enable: ");
  Serial.println((full_status[6]&0x40) >> 6);
  
  Serial.print("LED3 enable: ");
  Serial.println((full_status[6]&0x80) >> 7);
  // full_status[7] = 0xFF
}

void get_status(){
  Serial.println("Get Status");
  ncv7430.get_status(status);
  frame(status, 2);
  Serial.print("AD: ");
  Serial.println(status[0]&0x3F,HEX);
  
  Serial.print("LIN Error:   ");
  Serial.println(status[1]&0x01);
  
  Serial.print("Error LED1:  ");
  Serial.println((status[1]&0x02) >> 1);
  
  Serial.print("Error LED2:  ");
  Serial.println((status[1]&0x04) >> 2);
  
  Serial.print("Error LED3:  ");
  Serial.println((status[1]&0x08) >> 3);
  
  Serial.print("RETRY state: ");
  Serial.println((status[1]&0x10) >> 4);
  
  Serial.print("VBB Under Voltage: ");
  Serial.println((status[1]&0x20) >> 5);
  
  Serial.print("TW:          ");
  Serial.println((status[1]&0x40) >> 6);
  
  Serial.print("TSD:         ");
  Serial.println((status[1]&0x80) >> 7);
}

void get_color(){
  Serial.println("Get Color");
  ncv7430.get_color(color);
  frame(color, 8);
  Serial.print("AD: ");
  Serial.println(color[0]&0x3F,HEX);
  
  Serial.print("LED modulation value LED1[7:0]:  ");
  Serial.println(color[1],HEX);
  
  Serial.print("LED modulation value LED1[10:8]: ");
  Serial.println(color[2]&0x07,HEX);
  
  Serial.print("LED modulation overflow LED1:    ");
  Serial.println((color[2]&0x08) >> 3,HEX);
  
  Serial.print("LED modulation value LED2[7:0]:  ");
  Serial.println(color[3],HEX);
  
  Serial.print("LED modulation value LED2[10:8]: ");
  Serial.println(color[4]&0x07,HEX);
  
  Serial.print("LED modulation overflow LED2:    ");
  Serial.println((color[4]&0x08) >> 3,HEX);
  
  Serial.print("LED modulation value LED3[7:0]:  ");
  Serial.println(color[5],HEX);
  
  Serial.print("LED modulation value LED3[10:8]: ");
  Serial.println(color[6]&0x07,HEX);
  
  Serial.print("LED modulation overflow LED3:    ");
  Serial.println((color[6]&0x08) >> 3,HEX);
  
  Serial.print("Intensity:     ");
  Serial.println((color[6]&0xF0) >> 4,HEX);
  
  Serial.print("Fading Time:   ");
  Serial.println(color[7]&0x3F);
  
  Serial.print("Fading Slope:  ");
  Serial.println((color[7]&0x40) >> 6);
  
  Serial.print("Fading on/off: ");
  Serial.println((color[7]&0x80) >> 7);
}

void get_led_control(){
  Serial.println("Get LED Control");
  ncv7430.get_led_control(led_control);
  frame(led_control, 2);
  Serial.print("AD: ");
  Serial.println(led_control[0]&0x3F,HEX);
  
  Serial.print("LED1 enable:  ");
  Serial.println((led_control[1]&0x02) >> 1);
  
  Serial.print("LED2 enable:  ");
  Serial.println((led_control[1]&0x04) >> 2);
  
  Serial.print("LED3 enable:  ");
  Serial.println((led_control[1]&0x08) >> 3);
  
  Serial.print("TH_CONT:      ");
  Serial.println((led_control[1]&0x10) >> 4);
  
  Serial.print("LEDs on/off:  ");
  Serial.println((led_control[1]&0x20) >> 5);
  
  Serial.print("LED_MOD_FREQ: ");
  Serial.println((led_control[1]&0x40) >> 6);
  
  Serial.print("Cal enabled:  ");
  Serial.println((led_control[1]&0x80) >> 7);
}

void get_act_param1(){
  Serial.println("Get actual Param1");
  ncv7430.get_actual_param1(act_param1);
  frame(act_param1, 8);
  Serial.print("AD: ");
  Serial.println(act_param1[0]&0x3F,HEX);
  
  Serial.print("LED color value LED1: ");
  Serial.println(act_param1[1],HEX);
  
  Serial.print("LED color value LED2: ");
  Serial.println(act_param1[2],HEX);
  
  Serial.print("LED color value LED3: ");
  Serial.println(act_param1[3],HEX);
  
  Serial.print("LED modulation data a11: ");
  Serial.println(act_param1[4],HEX);
  
  Serial.print("LED modulation data a22: ");
  Serial.println(act_param1[5],HEX);
  
  Serial.print("LED modulation data a33: ");
  Serial.println(act_param1[5],HEX);
  
  Serial.print("Fading Time:   ");
  Serial.println(act_param1[7]&0x3F);
  
  Serial.print("Fading Slope:  ");
  Serial.println((act_param1[7]&0x40) >> 6);
  
  Serial.print("Fading on/off: ");
  Serial.println((act_param1[7]&0x80) >> 7);
}

void get_act_param2(){
  Serial.println("Get actual Param2");
  ncv7430.get_actual_param2(act_param2);
  frame(act_param2, 8);
  Serial.print("AD: ");
  Serial.println(act_param2[0]&0x3F,HEX);
  
  Serial.print("LED modulation data a12: ");
  Serial.println(act_param2[1],HEX);
  
  Serial.print("LED modulation data a13: ");
  Serial.println(act_param2[2],HEX);
  
  Serial.print("LED modulation data a21: ");
  Serial.println(act_param2[3],HEX);
  
  Serial.print("LED modulation data a23: ");
  Serial.println(act_param2[4],HEX);
  
  Serial.print("LED modulation data a31: ");
  Serial.println(act_param2[5],HEX);
  
  Serial.print("LED modulation data a32: ");
  Serial.println(act_param2[6],HEX);

  Serial.print("GROUP_ID:     ");
  Serial.println(act_param2[7]&0x0F);
  
  Serial.print("TH_CONT:      ");
  Serial.println((act_param2[7]&0x10) >> 4);
  
  // act_param2[7]&0x20 = 1
  Serial.print("LED_MOD_FREQ: ");
  Serial.println((act_param2[7]&0x40) >> 6);
  
  Serial.print("Cal enabled:  ");
  Serial.println((act_param2[7]&0x80) >> 7);
}

void frame(uint8_t* data, uint8_t lenght) {
  for (int i=0; i<lenght; i++) {
    if (data[i] < 0x10)
      Serial.print("0");
    
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void blinkLED() {
  ledState = !ledState;
  
  digitalWrite(LED_BUILTIN, ledState);
}