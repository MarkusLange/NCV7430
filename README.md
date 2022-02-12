# NCV7430
Library for the NCV7430 LIN LED Driver

Needs the Teensy_3.x_4.x_and_LC_LIN_Master as communication driver for the LIN Protocol

The Library handle the color as vector

Example:

//                  R    G    B

uint8_t rgb_r[3] = {255,   0,   0};

uint8_t rgb_g[3] = {  0, 255,   0};

uint8_t rgb_b[3] = {  0,   0, 255};

Since the "SK Pang Electronics LIN Bus Slave RGB LED Breakout Board" did not has the LEDs Red, Green ans Blue in the normal order connected to the IC,
the color sequence is adjusted inside the Library for known usage, for different sequences it has reassign.

Inside the ncv7430.cpp

// LED1 = 0, LED2 = 1, LED2 = 2

#define led_r 1

#define led_g 0

#define led_b 2
