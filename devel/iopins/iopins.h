#pragma once

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "calc.h"

/** Pin numbering reference */
#define D0 0
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define D8 8
#define D9 9
#define D10 10
#define D11 11
#define D12 12
#define D13 13
#define D14 14
#define D15 15
#define D16 16
#define D17 17
#define D18 18
#define D19 19
#define D20 20
#define D21 21
#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define A4 18
#define A5 19
#define A6 20
#define A7 21


/** Set pin direction */
#define set_dir(pin, d)  set_bit(_DDR_##pin, _PN_##pin, d)
void    set_dir_n(const uint8_t pin, const uint8_t d);


/** Configure pin as input */
#define as_input(pin)    cbi(_DDR_##pin, _PN_##pin)
void    as_input_n(const uint8_t pin);


/** Configure pin as input, with pull-up enabled */
#define as_input_pu(pin) { as_input(pin); set_high(pin); }
void    as_input_pu_n(const uint8_t pin);


/** Configure pin as output */
#define as_output(pin)   sbi(_DDR_##pin, _PN_##pin)
void    as_output_n(const uint8_t pin);


/** Write value to a pin */
#define set_pin(pin, v) set_bit(_PORT_##pin, _PN_##pin, v)
void    set_pin_n(const uint8_t pin, const uint8_t v);


/** Write 0 to a pin */
#define set_low(pin)    cbi(_PORT_##pin, _PN_##pin)
void    set_low_n(const uint8_t pin);


/** Write 1 to a pin */
#define set_high(pin)   sbi(_PORT_##pin, _PN_##pin)
void    set_high_n(const uint8_t pin);


/** Toggle a pin state */
#define toggle_pin(pin)   sbi(_PIN_##pin, _PN_##pin)
void    toggle_pin_n(const uint8_t pin);


/** Read a pin value */
#define get_pin(pin)  get_bit(_PIN_##pin, _PN_##pin)
bool    get_pin_n(const uint8_t pin);


/** CHeck if pin is low */
#define is_low(pin)   (get_pin(pin) == 0)
bool    is_low_n(const uint8_t pin);


/** CHeck if pin is high */
#define is_high(pin)  (get_pin(pin) != 0)
bool    is_high_n(const uint8_t pin);



// Helper macros

#define _PORT_0  PORTD
#define _PORT_1  PORTD
#define _PORT_2  PORTD
#define _PORT_3  PORTD
#define _PORT_4  PORTD
#define _PORT_5  PORTD
#define _PORT_6  PORTD
#define _PORT_7  PORTD
#define _PORT_8  PORTB
#define _PORT_9  PORTB
#define _PORT_10 PORTB
#define _PORT_11 PORTB
#define _PORT_12 PORTB
#define _PORT_13 PORTB
#define _PORT_14 PORTC
#define _PORT_15 PORTC
#define _PORT_16 PORTC
#define _PORT_17 PORTC
#define _PORT_18 PORTC
#define _PORT_19 PORTC
#define _PORT_20 PORTC
#define _PORT_21 PORTC

#define _PIN_0  PIND
#define _PIN_1  PIND
#define _PIN_2  PIND
#define _PIN_3  PIND
#define _PIN_4  PIND
#define _PIN_5  PIND
#define _PIN_6  PIND
#define _PIN_7  PIND
#define _PIN_8  PINB
#define _PIN_9  PINB
#define _PIN_10 PINB
#define _PIN_11 PINB
#define _PIN_12 PINB
#define _PIN_13 PINB
#define _PIN_14 PINC
#define _PIN_15 PINC
#define _PIN_16 PINC
#define _PIN_17 PINC
#define _PIN_18 PINC
#define _PIN_19 PINC
#define _PIN_20 PINC
#define _PIN_21 PINC

#define _DDR_0  DDRD
#define _DDR_1  DDRD
#define _DDR_2  DDRD
#define _DDR_3  DDRD
#define _DDR_4  DDRD
#define _DDR_5  DDRD
#define _DDR_6  DDRD
#define _DDR_7  DDRD
#define _DDR_8  DDRB
#define _DDR_9  DDRB
#define _DDR_10 DDRB
#define _DDR_11 DDRB
#define _DDR_12 DDRB
#define _DDR_13 DDRB
#define _DDR_14 DDRC
#define _DDR_15 DDRC
#define _DDR_16 DDRC
#define _DDR_17 DDRC
#define _DDR_18 DDRC
#define _DDR_19 DDRC
#define _DDR_20 DDRC
#define _DDR_21 DDRC

#define _PN_0  0
#define _PN_1  1
#define _PN_2  2
#define _PN_3  3
#define _PN_4  4
#define _PN_5  5
#define _PN_6  6
#define _PN_7  7
#define _PN_8  0
#define _PN_9  1
#define _PN_10 2
#define _PN_11 3
#define _PN_12 4
#define _PN_13 5
#define _PN_14 0
#define _PN_15 1
#define _PN_16 2
#define _PN_17 3
#define _PN_18 4
#define _PN_19 5
#define _PN_20 6
#define _PN_21 7
