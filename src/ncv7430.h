#include <Arduino.h>
#include "lin_bus.h"

class NCV7430 : public LIN {

private:
  //
  
public:
  static const int BIT_RATE = 19200;
  NCV7430(HardwareSerial* stream, uint16_t baudrate = BIT_RATE);
  void init();
  void init(uint8_t led_on, uint8_t* leds);
  void set_color(uint8_t intensity, uint8_t* color);
  void set_led_color(uint8_t update_color, uint8_t fading_time, uint8_t intensity, uint8_t led_on, uint8_t fading_slope, uint8_t fading, uint8_t* color);
  void set_led_control(uint8_t cal_en, uint8_t led_mod_frq, uint8_t led_on, uint8_t th_count, uint8_t* leds);
  void set_color_short(uint8_t* color);
  void set_intensity(uint8_t intensity, uint8_t fading_time);
  void set_primary_cal_param(uint8_t* prim_cal_param);
  void set_secondary_cal_param(uint8_t* sec_cal_param);
  void set_otp_param(uint8_t otp_adress_pionter, uint8_t otp_contents);
  void sleep();
  int get_full_status(uint8_t* recv_data);
  int get_actual_param1(uint8_t* recv_data);
  int get_actual_param2(uint8_t* recv_data);
  int get_otp_param1(uint8_t* recv_data);
  int get_otp_param2(uint8_t* recv_data);
  int get_status(uint8_t* recv_data);
  int get_color(uint8_t* recv_data);
  int get_led_control(uint8_t* recv_data);
  
  void set_color_fade(uint8_t intensity, float fadingtime, uint8_t* color1, uint8_t* color2);
  void set_intensity_fade(uint8_t intensity1, uint8_t intensity2, float fadingtime, uint8_t* color);
  
  void set_node_addressing(uint8_t address);
  void set_group_addressing(uint8_t group);
  void set_fading_slope(uint8_t slope);
};