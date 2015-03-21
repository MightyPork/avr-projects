#pragma once

/**
  An implementation of button debouncer.

  First, the system must be initialized - even before including:

      #define DEBO_CHANNELS 2
      #define DEBO_TICKS 5

      #inclue "lib/debounce.h"

  A pin is registered like this:

      #define BTN1 B,0
      #define BTN2 B,1

      debo_add(BTN0);  // The function returns number assigned to the pin (0, 1, ...)
      debo_add_rev(BTN1);  // active low
      debo_register(&PINB, PB2, 0);  // direct access - register, pin & invert

  Then periodically call the tick function (perhaps in a timer interrupt):

      debo_tick();

  To check if input is active, use

      debo_get_pin(0); // state of input registered as #0
      debo_get_pin(1); // state of input registered as #1
*/

#include <avr/io.h>
#include <stdbool.h>

#include "calc.h"
#include "pins.h"


// Number of pins to debounce
#ifndef DEBO_CHANNELS
# error "DEBO_CHANNELS not defined!"
#endif

#ifndef DEBO_TICKS
# warning "DEBO_TICKS not defined, defaulting to 5!"
# define DEBO_TICKS 5
#endif


/* Internal deboucer entry */
typedef struct {
	PORT_P reg;    // pointer to IO register
	uint8_t bit;   // bits 6 and 7 of this hold "state" & "invert" flag
	uint8_t count; // number of ticks this was in the new state
} debo_slot_t;

/** Debounce data array */
debo_slot_t debo_slots[DEBO_CHANNELS];
uint8_t debo_next_slot = 0;

/** Define a debounced pin (must be IO!) */

#define debo_add_rev(io) debo_register(&io2pin(io_pack(io)), io2n(io_pack(io)), 1)
#define debo_add(io)  debo_register(&io2pin(io_pack(io)), io2n(io_pack(io)), 0)

uint8_t debo_register(PORT_P reg, uint8_t bit, bool invert)
{
	debo_slots[debo_next_slot] = (debo_slot_t){
		.reg = reg,
		.bit = bit | ((invert & 1) << 7) | (get_bit_p(reg, bit) << 6), // bit 7 = invert, bit 6 = state
		.count = 0,
	};

	return debo_next_slot++;
}


/** Check debounced pins, should be called periodically. */
void debo_tick()
{
	for (uint8_t i = 0; i < debo_next_slot; i++) {
		// current pin value (right 3 bits, xored with inverse bit)
		bool value = get_bit_p(debo_slots[i].reg, debo_slots[i].bit & 0x7);

		if (value != get_bit(debo_slots[i].bit, 6)) {

			// different pin state than last recorded state
			if (debo_slots[i].count < DEBO_TICKS) {
				debo_slots[i].count++;
			} else {
				// overflown -> latch value
				set_bit(debo_slots[i].bit, 6, value); // set state bit
				debo_slots[i].count = 0;
			}
		} else {
			debo_slots[i].count = 0; // reset the counter
		}
	}
}


/** Get a value of debounced pin */
#define debo_get_pin(i) (get_bit(debo_slots[i].bit, 6) ^ get_bit(debo_slots[i].bit, 7))
