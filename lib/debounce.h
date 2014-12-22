#pragma once

/**
  An implementation of button debouncer.

  First, the system must be initialized:

      debounce_init();

  A pin is registered for debouncing by calling

      #define BTN0 B,0
      #define BTN1 B,1

      debounce_register(0, BTN0);
      debounce_register(1, BTN1);

  Then periodically the tick function must be called:

      debounce_tick();

  To check if pin is high, use

      debounce_get_pin(0); // registered as #0
      debounce_get_pin(1); // registered as #1
*/

#include "pins.h"
#include "calc.h"
#include "avr/io.h"

/** Number of ticks the pin must be in given state */
#ifndef DEBOUNCE_TICKS
# define DEBOUNCE_TICKS 5
#endif

/** Max number of pins observed */
#ifndef DEBOUNCE_SLOT_COUNT
#define DEBOUNCE_SLOT_COUNT 16
#endif


/* Internal deboucer entry */
typedef struct {
	PORT_P port;
	PIN_N bit;
	uint8_t count;
	bool state;
} debounce_slot_t;


/** Debounce data array */
debounce_slot_t debounce_slots[DEBOUNCE_SLOT_COUNT];


/** Init the debounce slots table */
void debounce_init()
{
	for (uint8_t i = 0; i < DEBOUNCE_SLOT_COUNT; i++) {
		debounce_slots[i] = {
			.port = (PORT_P)0;
			.bit = 0;
			.state = 0;
			.count = 0;
		};
	}
}


/** Define a debounced pin (must be IO!) */
inline void debounce_register_real(uint8_t number, PORT_P port, PIN_N bit)
{
	debounce_slots[number] = {
		.port = port;
		.bit = bit;
		.count = 0;
		.state = 0;
	};
}

#define debounce_register(number, io) debounce_register_real((number), &io2port(io), io2n(io))


/** Check debounced pins, should be called periodically. */
void debounce_tick()
{
	for (uint8_t i = 0; i < DEBOUNCE_SLOT_COUNT; i++) {

		if (debounce_slots[i].port == 0) continue; // slot is unused

		// current pin value
		bool value = read_bit_p(debounce_slots[i].port, debounce_slots[i].bit);

		if (value != debounce_slots[i].state) {

			// different pin state than last recorded state
			if (debounce_slots[i].count < DEBOUNCE_TICKS) {
				// increment
				if (++debounce_slots[i].count == DEBOUNCE_TICKS) {
					// overflown -> latch value
					debounce_slots[i].state = value;
				}
			}

		} else {
			debounce_slots[i].count = 0; // reset the counter
		}
	}
}


/** Get a value of debounced pin */
inline bool debounce_get_pin(uint8_t number)
{
	return debounce_slots[number].state;
}
