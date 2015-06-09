#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "calc.h"
#include "iopins.h"


void set_dir_n(const uint8_t pin, const uint8_t d)
{
	switch(pin) {
		case 0: set_dir(0, d); return;
		case 1: set_dir(1, d); return;
		case 2: set_dir(2, d); return;
		case 3: set_dir(3, d); return;
		case 4: set_dir(4, d); return;
		case 5: set_dir(5, d); return;
		case 6: set_dir(6, d); return;
		case 7: set_dir(7, d); return;
		case 8: set_dir(8, d); return;
		case 9: set_dir(9, d); return;
		case 10: set_dir(10, d); return;
		case 11: set_dir(11, d); return;
		case 12: set_dir(12, d); return;
		case 13: set_dir(13, d); return;
		case 14: set_dir(14, d); return;
		case 15: set_dir(15, d); return;
		case 16: set_dir(16, d); return;
		case 17: set_dir(17, d); return;
		case 18: set_dir(18, d); return;
		case 19: set_dir(19, d); return;
		case 20: set_dir(20, d); return;
		case 21: set_dir(21, d); return;
	}
}

void as_input_n(const uint8_t pin)
{
	switch(pin) {
		case 0: as_input(0); return;
		case 1: as_input(1); return;
		case 2: as_input(2); return;
		case 3: as_input(3); return;
		case 4: as_input(4); return;
		case 5: as_input(5); return;
		case 6: as_input(6); return;
		case 7: as_input(7); return;
		case 8: as_input(8); return;
		case 9: as_input(9); return;
		case 10: as_input(10); return;
		case 11: as_input(11); return;
		case 12: as_input(12); return;
		case 13: as_input(13); return;
		case 14: as_input(14); return;
		case 15: as_input(15); return;
		case 16: as_input(16); return;
		case 17: as_input(17); return;
		case 18: as_input(18); return;
		case 19: as_input(19); return;
		case 20: as_input(20); return;
		case 21: as_input(21); return;
	}
}


void as_input_pu_n(const uint8_t pin)
{
	switch(pin) {
		case 0: as_input_pu(0); return;
		case 1: as_input_pu(1); return;
		case 2: as_input_pu(2); return;
		case 3: as_input_pu(3); return;
		case 4: as_input_pu(4); return;
		case 5: as_input_pu(5); return;
		case 6: as_input_pu(6); return;
		case 7: as_input_pu(7); return;
		case 8: as_input_pu(8); return;
		case 9: as_input_pu(9); return;
		case 10: as_input_pu(10); return;
		case 11: as_input_pu(11); return;
		case 12: as_input_pu(12); return;
		case 13: as_input_pu(13); return;
		case 14: as_input_pu(14); return;
		case 15: as_input_pu(15); return;
		case 16: as_input_pu(16); return;
		case 17: as_input_pu(17); return;
		case 18: as_input_pu(18); return;
		case 19: as_input_pu(19); return;
		case 20: as_input_pu(20); return;
		case 21: as_input_pu(21); return;
	}
}


void as_output_n(const uint8_t pin)
{
	switch(pin) {
		case 0: as_output(0); return;
		case 1: as_output(1); return;
		case 2: as_output(2); return;
		case 3: as_output(3); return;
		case 4: as_output(4); return;
		case 5: as_output(5); return;
		case 6: as_output(6); return;
		case 7: as_output(7); return;
		case 8: as_output(8); return;
		case 9: as_output(9); return;
		case 10: as_output(10); return;
		case 11: as_output(11); return;
		case 12: as_output(12); return;
		case 13: as_output(13); return;
		case 14: as_output(14); return;
		case 15: as_output(15); return;
		case 16: as_output(16); return;
		case 17: as_output(17); return;
		case 18: as_output(18); return;
		case 19: as_output(19); return;
		case 20: as_output(20); return;
		case 21: as_output(21); return;
	}
}

void set_pin_n(const uint8_t pin, const uint8_t v)
{
	switch(pin) {
		case 0: set_pin(0, v); return;
		case 1: set_pin(1, v); return;
		case 2: set_pin(2, v); return;
		case 3: set_pin(3, v); return;
		case 4: set_pin(4, v); return;
		case 5: set_pin(5, v); return;
		case 6: set_pin(6, v); return;
		case 7: set_pin(7, v); return;
		case 8: set_pin(8, v); return;
		case 9: set_pin(9, v); return;
		case 10: set_pin(10, v); return;
		case 11: set_pin(11, v); return;
		case 12: set_pin(12, v); return;
		case 13: set_pin(13, v); return;
		case 14: set_pin(14, v); return;
		case 15: set_pin(15, v); return;
		case 16: set_pin(16, v); return;
		case 17: set_pin(17, v); return;
		case 18: set_pin(18, v); return;
		case 19: set_pin(19, v); return;
		case 20: set_pin(20, v); return;
		case 21: set_pin(21, v); return;
	}
}

void set_low_n(const uint8_t pin)
{
	switch(pin) {
		case 0: set_low(0); return;
		case 1: set_low(1); return;
		case 2: set_low(2); return;
		case 3: set_low(3); return;
		case 4: set_low(4); return;
		case 5: set_low(5); return;
		case 6: set_low(6); return;
		case 7: set_low(7); return;
		case 8: set_low(8); return;
		case 9: set_low(9); return;
		case 10: set_low(10); return;
		case 11: set_low(11); return;
		case 12: set_low(12); return;
		case 13: set_low(13); return;
		case 14: set_low(14); return;
		case 15: set_low(15); return;
		case 16: set_low(16); return;
		case 17: set_low(17); return;
		case 18: set_low(18); return;
		case 19: set_low(19); return;
		case 20: set_low(20); return;
		case 21: set_low(21); return;
	}
}

void set_high_n(const uint8_t pin)
{
	switch(pin) {
		case 0: set_high(0); return;
		case 1: set_high(1); return;
		case 2: set_high(2); return;
		case 3: set_high(3); return;
		case 4: set_high(4); return;
		case 5: set_high(5); return;
		case 6: set_high(6); return;
		case 7: set_high(7); return;
		case 8: set_high(8); return;
		case 9: set_high(9); return;
		case 10: set_high(10); return;
		case 11: set_high(11); return;
		case 12: set_high(12); return;
		case 13: set_high(13); return;
		case 14: set_high(14); return;
		case 15: set_high(15); return;
		case 16: set_high(16); return;
		case 17: set_high(17); return;
		case 18: set_high(18); return;
		case 19: set_high(19); return;
		case 20: set_high(20); return;
		case 21: set_high(21); return;
	}
}


void toggle_pin_n(const uint8_t pin)
{
	switch(pin) {
		case 0: toggle_pin(0); return;
		case 1: toggle_pin(1); return;
		case 2: toggle_pin(2); return;
		case 3: toggle_pin(3); return;
		case 4: toggle_pin(4); return;
		case 5: toggle_pin(5); return;
		case 6: toggle_pin(6); return;
		case 7: toggle_pin(7); return;
		case 8: toggle_pin(8); return;
		case 9: toggle_pin(9); return;
		case 10: toggle_pin(10); return;
		case 11: toggle_pin(11); return;
		case 12: toggle_pin(12); return;
		case 13: toggle_pin(13); return;
		case 14: toggle_pin(14); return;
		case 15: toggle_pin(15); return;
		case 16: toggle_pin(16); return;
		case 17: toggle_pin(17); return;
		case 18: toggle_pin(18); return;
		case 19: toggle_pin(19); return;
		case 20: toggle_pin(20); return;
		case 21: toggle_pin(21); return;
	}
}


bool get_pin_n(const uint8_t pin)
{
	switch(pin) {
		case 0: return get_pin(0);
		case 1: return get_pin(1);
		case 2: return get_pin(2);
		case 3: return get_pin(3);
		case 4: return get_pin(4);
		case 5: return get_pin(5);
		case 6: return get_pin(6);
		case 7: return get_pin(7);
		case 8: return get_pin(8);
		case 9: return get_pin(9);
		case 10: return get_pin(10);
		case 11: return get_pin(11);
		case 12: return get_pin(12);
		case 13: return get_pin(13);
		case 14: return get_pin(14);
		case 15: return get_pin(15);
		case 16: return get_pin(16);
		case 17: return get_pin(17);
		case 18: return get_pin(18);
		case 19: return get_pin(19);
		case 20: return get_pin(20);
		case 21: return get_pin(21);
	}
	return false;
}


bool is_low_n(const uint8_t pin)
{
	return !get_pin_n(pin);
}


bool is_high_n(const uint8_t pin)
{
	return get_pin_n(pin);
}
