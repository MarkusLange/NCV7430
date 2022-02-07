#include <Arduino.h>
#include "ncv7430.h"
#include "lin_bus.h"

#define fading_off 0
#define fading_on  1

#define fading_slope_logarithmic 0
#define fading_slope_linear      1

typedef struct {
  // 0 means group addressing.
  // 1 means single node addressing. The target node address is indicated by the AD[5:0] bit
  uint8_t broad = 1;
  // AD0 − AD5 NCV7430 address programming bits.
  uint8_t AD = 0;
  // GROUP_ID 2^4 = 16 Groups [0..15] in 4 bits
  uint8_t group_id = 0;
  // selectable in two byte
  uint16_t group = 1 << group_id;
  uint8_t c_group = group_id & 0x03;
  uint8_t i_group = (group_id & 0x0C) >> 2;
  // 0 the real value for the color setting is written into the LED modulation register.
  // 1 the received 8 bit values are first corrected by the matrix calculation
  uint8_t cal_en = 0;
  // 0 LED modulation frequency − 122 Hz
  // 1 LED modulation frequency − 244 Hz
  uint8_t led_mod_frq = 1;
  // 0 Thermal Control Disabled.
  // 1 Thermal Control Enabled.
  uint8_t th_count = 1;
  // 0 All LEDs OFF
  // 1 All LEDs ON if individual LEDx ENABLE is set to "1"
  uint8_t led_on = 1;
  // 0 LEDx OFF
  // 1 LEDx ON
  uint8_t leds[3] = {1, 1, 1};
  // 00 immediate update
  // 01 store and do not update
  // 10 update to the already stored values
  // 11 discard
  uint8_t update_color = 0;
  // 6−bit unsigned: 0 .. 6.3 seconds in resolution steps of 0.1 secs
  uint8_t fading_time = 0;
  // 0 Fading slope logarithmic
  // 1 Fading slope linear
  uint8_t fading_slope = fading_slope_linear;
  // 0 Fading off
  // 1 Fading on
  uint8_t fading = fading_off;
} basic_settings;

basic_settings setting;

#define READING_FRAME                         0x20
#define GET_LED_RESPONSE_FRAME                0x10
#define GET_STATUS_RESPONSE_FRAME             0x11
#define GET_COLOR_RESPONSE_FRAME              0x12

#define SET_LED_CONTROL_WRITING_FRAME         0x23
#define SET_LED_COLOR_WRITING_FRAME           0x24
#define SET_PRIMARY_CAL_PARAM_WRITING_FRAME   0x25
#define SET_SECONDARY_CAL_PARAM_WRITING_FRAME 0x36
#define SET_OTP_PARAM_WRITING_FRAME           0x27

#define SET_INTENSITY_SHORT_WRITING_FRAME     0x2E
#define SET_COLOR_SHORT_WRITING_FRAME         0x2F

#define COMMAND_FRAME_MASTER                  0x3C
#define RESPONSE_FRAME_SLAVE                  0x3D

// LED1 = 0, LED2 = 1, LED2 = 2
#define led_r 1
#define led_g 0
#define led_b 2

#define len 8
uint8_t lin_data[len];

uint8_t get_full_status_frame[]   = {0x80, 0x81, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t get_actual_param1_frame[] = {0x80, 0x82, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t get_actual_param2_frame[] = {0x80, 0x83, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t get_otp_param1_frame[]    = {0x80, 0x84, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t get_otp_param2_frame[]    = {0x80, 0x85, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t get_status_frame[]        = {0xC0, 0xFF};

uint8_t sleep_frame[]             = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

NCV7430::NCV7430(HardwareSerial* stream, uint16_t baudrate) : LIN(stream, baudrate) {
  //
}

void NCV7430::init() {
  set_led_control(setting.cal_en, setting.led_mod_frq, setting.led_on, setting.th_count, setting.leds);
}

void NCV7430::init(uint8_t led_on, uint8_t* leds) {
  set_led_control(setting.cal_en, setting.led_mod_frq, led_on, setting.th_count, leds);
}

void NCV7430::set_led_control(uint8_t cal_en, uint8_t led_mod_frq, uint8_t led_on, uint8_t th_count, uint8_t* leds) {
  lin_data[0] = 0x40 | ((setting.broad & 0x01) << 7) | (setting.AD & 0x3F);
  lin_data[1] = 0x00 | (setting.group & 0xFF);
  lin_data[2] = 0x00 | (setting.group >> 2);
  lin_data[3] = 0x01 | ((cal_en & 0x01) << 7) | ((led_mod_frq & 0x01) << 6) | ((led_on & 0x01) << 5) | ((th_count & 0x01) << 4) | (leds[led_b] << 3) | (leds[led_g] << 2) | (leds[led_r] << 1);
  
  LIN::order(SET_LED_CONTROL_WRITING_FRAME, lin_data, 4);
}

void NCV7430::set_node_addressing(uint8_t node){
  setting.group_id = 0;
  setting.broad = 1;
  setting.AD = node;
}

void NCV7430::set_group_addressing(uint8_t group){
  setting.group_id = group;
  setting.broad = 0;
  setting.AD = 0;
}

void NCV7430::set_fading_slope(uint8_t slope){
  setting.fading_slope = slope;
}

void NCV7430::set_color(uint8_t intensity, uint8_t* color) {
  set_led_color(setting.update_color, setting.fading_time, intensity, setting.led_on, setting.fading_slope, setting.fading, color);
}

void NCV7430::set_led_color(uint8_t update_color, uint8_t fading_time, uint8_t intensity, uint8_t led_on, uint8_t fading_slope, uint8_t fading, uint8_t* color) {
  lin_data[0] = 0x40 | ((setting.broad & 0x01) << 7) | (setting.AD & 0x3F);
  lin_data[1] = 0x00 | (setting.group & 0xFF);
  lin_data[2] = 0x00 | (setting.group >> 2);
  lin_data[3] = 0x00 | ((update_color & 0x03) << 6) | (fading_time & 0x3F);
  lin_data[4] = 0x10 | ((fading & 0x01) << 7) | ((fading_slope & 0x01) << 6) | ((led_on & 0x01) << 5) | (intensity & 0x0F);
  lin_data[5] = 0x00 | color[led_r];
  lin_data[6] = 0x00 | color[led_g];
  lin_data[7] = 0x00 | color[led_b];
  
  LIN::order(SET_LED_COLOR_WRITING_FRAME, lin_data, 8);
}

void NCV7430::set_color_short(uint8_t* color) {
  lin_data[0] = 0x00 | color[led_r];
  lin_data[1] = 0x00 | color[led_g];
  lin_data[2] = 0x00 | color[led_b];
  lin_data[3] = 0xF0 | (1 << setting.c_group);
  
  LIN::order(SET_COLOR_SHORT_WRITING_FRAME, lin_data, 4);
}

void NCV7430::set_color_fade(uint8_t intensity, float fadingtime, uint8_t* color1, uint8_t* color2) {
  set_color(intensity, color1);
  
  uint8_t fading_time = fadingtime * 10;
  // Fading Slope must be set to "1" for color control (only Linear is allowed).
  set_led_color(setting.update_color, fading_time, intensity, setting.led_on, fading_slope_linear, fading_on, color2);
}

void NCV7430::set_intensity(uint8_t intensity, uint8_t fading_time) {
  lin_data[0] = 0x00 | (0x10 << setting.i_group) | (intensity & 0x0F);
  lin_data[1] = 0xC0 | (fading_time & 0x3F);
  
  LIN::order(SET_INTENSITY_SHORT_WRITING_FRAME, lin_data, 2);
}

void NCV7430::set_intensity_fade(uint8_t intensity1, uint8_t intensity2, float fadingtime, uint8_t* color) {
  set_color(intensity1, color);
  
  uint8_t fading_time = fadingtime * 10;
  set_led_color(setting.update_color, fading_time, intensity2, setting.led_on, setting.fading_slope, fading_on, color);
}

void NCV7430::set_primary_cal_param(uint8_t* prim_cal_param) {
  lin_data[0] = 0xC0 | (setting.AD & 0x3F);
  lin_data[1] = 0x00 | prim_cal_param[0];
  lin_data[2] = 0x00 | prim_cal_param[1];
  lin_data[3] = 0x00 | prim_cal_param[2];
  
  LIN::order(SET_PRIMARY_CAL_PARAM_WRITING_FRAME, lin_data, 4);
}

void NCV7430::set_secondary_cal_param(uint8_t* sec_cal_param) {
  lin_data[0] = 0xC0 | (setting.AD & 0x3F);
  lin_data[1] = 0x00 | sec_cal_param[0];
  lin_data[2] = 0x00 | sec_cal_param[1];
  lin_data[3] = 0x00 | sec_cal_param[2];
  lin_data[4] = 0x00 | sec_cal_param[3];
  lin_data[5] = 0x00 | sec_cal_param[4];
  lin_data[6] = 0x00 | sec_cal_param[5];
  lin_data[7] = 0xFF;
  
  LIN::order(SET_SECONDARY_CAL_PARAM_WRITING_FRAME, lin_data, 8);
}

void NCV7430::set_otp_param(uint8_t otp_adress_pionter, uint8_t otp_contents) {
  // prevent LOCKBT1 & CMDSOFF from setting
  if (otp_adress_pionter == 0x03)
    otp_contents = otp_contents & 0x3F;
  // prevent LOCKBT2 from setting
  if (otp_adress_pionter == 0x0E)
    otp_contents = otp_contents & 0x7F;
  
  lin_data[0] = 0xC0 | (setting.AD & 0x3F);
  lin_data[1] = 0xFF;
  lin_data[2] = 0xF0 | (otp_adress_pionter & 0x0F);
  lin_data[3] = 0x00 | otp_contents;
  
  LIN::order(SET_OTP_PARAM_WRITING_FRAME, lin_data, 4);
}

void NCV7430::sleep() {
  LIN::order(COMMAND_FRAME_MASTER, sleep_frame, 8);
}

int NCV7430::get_full_status(uint8_t* recv_data) {
  get_full_status_frame[2] = 0xC0 | (setting.AD & 0x3F);
  LIN::order(COMMAND_FRAME_MASTER, get_full_status_frame, 8);
  
  return LIN::response(RESPONSE_FRAME_SLAVE, recv_data, 8);
}

int NCV7430::get_actual_param1(uint8_t* recv_data) {
  get_actual_param1_frame[2] = 0xC0 | (setting.AD & 0x3F);
  LIN::order(COMMAND_FRAME_MASTER, get_actual_param1_frame, 8);
  
  return LIN::response(RESPONSE_FRAME_SLAVE, recv_data, 8);
}

int NCV7430::get_actual_param2(uint8_t* recv_data) {
  get_actual_param2_frame[2] = 0xC0 | (setting.AD & 0x3F);
  LIN::order(COMMAND_FRAME_MASTER, get_actual_param2_frame, 8);
  
  return LIN::response(RESPONSE_FRAME_SLAVE, recv_data, 8);
}

int NCV7430::get_otp_param1(uint8_t* recv_data) {
  get_otp_param1_frame[2] = 0xC0 | (setting.AD & 0x3F);
  LIN::order(COMMAND_FRAME_MASTER, get_otp_param1_frame, 8);
  
  return LIN::response(RESPONSE_FRAME_SLAVE, recv_data, 8);
}

int NCV7430::get_otp_param2(uint8_t* recv_data) {
  get_otp_param2_frame[2] = 0xC0 | (setting.AD & 0x3F);
  LIN::order(COMMAND_FRAME_MASTER, get_otp_param2_frame, 8);
  
  return LIN::response(RESPONSE_FRAME_SLAVE, recv_data, 8);
}

int NCV7430::get_status(uint8_t* recv_data) {
  get_status_frame[0] = 0xC0 | (setting.AD & 0x3F);
  LIN::order(READING_FRAME, get_status_frame, 2);
  
  return LIN::response(GET_STATUS_RESPONSE_FRAME, recv_data, 2);
}

int NCV7430::get_color(uint8_t* recv_data) {
  get_status_frame[0] = 0xC0 | (setting.AD & 0x3F);
  LIN::order(READING_FRAME, get_status_frame, 2);
  
  return LIN::response(GET_COLOR_RESPONSE_FRAME, recv_data, 8);
}

int NCV7430::get_led_control(uint8_t* recv_data) {
  get_status_frame[0] = 0xC0 | (setting.AD & 0x3F);
  LIN::order(READING_FRAME, get_status_frame, 2);
  
  return LIN::response(GET_LED_RESPONSE_FRAME, recv_data, 2);
}