WS2812B RGB LED strip driving code
==================================

This code is meant for 16MHz Arduino, but should work on any 16MHz AVR. It is in plain C, not the Arduino language.

All you need is `ws_driver.c` and `ws_driver.h`, and `config.c` where you define the port / pin information.

Connection
----------

Digital pin 8 on Arduino (PB0) is connected via a 200-500 R to WS2812B data input. In parallel to the LED strip is a 500-1000 uF capacitor that takes care of smoothening the power supply. Without the cap, the LEDs will flicker.

It is recommended to use stronger power supply for the LED strip, a 30-led one takes around to 1A when fully powered.
