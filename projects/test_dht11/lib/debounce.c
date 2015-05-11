#include <avr/io.h>
#include <stdbool.h>

#include "debounce.h"
#include "calc.h"
#include "iopins.h"
#include "debo_config.h"

/** Debounce data array */
uint8_t debo_next_slot = 0;

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
