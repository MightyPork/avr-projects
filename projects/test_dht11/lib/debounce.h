#pragma once

//
//  An implementation of button debouncer.
//
//  ----
//
//  You must provide a config file debo_config.h (next to your main.c)
//
//  A pin is registered like this:
//
//    #define BTN1 12 // pin D12
//    #define BTN2 13
//
//    debo_add(BTN0);  // The function returns number assigned to the pin (0, 1, ...)
//    debo_add_rev(BTN1);  // active low
//    debo_register(&PINB, PB2, 0);  // direct access - register, pin & invert
//
//  Then periodically call the tick function (perhaps in a timer interrupt):
//
//    debo_tick();
//
//  To check if input is active, use
//
//    debo_get_pin(0); // state of input #0 (registered first)
//    debo_get_pin(1); // state of input #1 (registered second)
//


#include <avr/io.h>
#include <stdbool.h>

#include "calc.h"
#include "pins.h"

// Your config file
#include "debo_config.h"
/*
	#define DEBO_CHANNELS 2
	#define DDEBO_TICKS 5
*/


/* Internal deboucer entry */
typedef struct {
	PORT_P reg;    // pointer to IO register
	uint8_t bit;   // bits 6 and 7 of this hold "state" & "invert" flag
	uint8_t count; // number of ticks this was in the new state
} debo_slot_t;

debo_slot_t debo_slots[DEBO_CHANNELS];

/** Add a pin for debouncing (must be used with constant args) */
#define debo_add_rev(pin) debo_register(&_pin(pin), _pn(pin), 1)
#define debo_add(pin)  debo_register(&_pin(pin), _pn(pin), 0)

/** Add a pin for debouncing (low level function) */
uint8_t debo_register(PORT_P pin_reg_pointer, uint8_t bit, bool invert);

/** Check debounced pins, should be called periodically. */
void debo_tick();

/** Get a value of debounced pin */
#define debo_get_pin(i) (get_bit(debo_slots[i].bit, 6) ^ get_bit(debo_slots[i].bit, 7))
