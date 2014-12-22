#include <avr/io.h>
#include <util/delay_basic.h>
#include <stdint.h>

#include "extra_delays.h"
#include "pins.h"
#include "ws_driver2.h"



/** Latch and display the RGB values */
void ws_show_real(PORT_P portp, BIT_N pin)
{
	cbi_p(portp, pin);
	delay_ns_c(WS_T_LATCH, 0);
}


/** Send one byte to the RGB strip */
void ws_send_byte_real(PORT_P portp, BIT_N pin, uint8_t bb)
{
	for (int8_t i = 7; i >= 0; --i) {
		if (bb & (1 << i)) {
			// send ONE
			sbi_p(portp, pin);
			delay_ns_c(WS_T_1H, -2);
			cbi_p(portp, pin);
			delay_ns_c(WS_T_1L, -10); // compensation for overhead
		} else {
			// send ZERO
			sbi_p(portp, pin);
			delay_ns_c(WS_T_0H, -2);
			cbi_p(portp, pin);
			delay_ns_c(WS_T_0L, -10);
		}
	}
}


/** Send RGB color to the strip */
void ws_send_rgb_real(PORT_P portp, BIT_N pin, uint8_t r, uint8_t g, uint8_t b)
{
	int8_t i;

	// manualy inlined, because `inline` doesn't work with delays.

	// GREEN
	for (i = 7; i >= 0; --i) {
		if (g & (1 << i)) {
			// send ONE
			sbi_p(portp, pin);
			delay_ns_c(WS_T_1H, -2);
			cbi_p(portp, pin);
			delay_ns_c(WS_T_1L, -10);
		} else {
			// send ZERO
			sbi_p(portp, pin);
			delay_ns_c(WS_T_0H, -2);
			cbi_p(portp, pin);
			delay_ns_c(WS_T_0L, -10);
		}
	}

	// RED
	for (i = 7; i >= 0; --i) {
		if (r & (1 << i)) {
			// send ONE
			sbi_p(portp, pin);
			delay_ns_c(WS_T_1H, -2);
			cbi_p(portp, pin);
			delay_ns_c(WS_T_1L, -10);
		} else {
			// send ZERO
			sbi_p(portp, pin);
			delay_ns_c(WS_T_0H, -2);
			cbi_p(portp, pin);
			delay_ns_c(WS_T_0L, -10);
		}
	}

	// BLUE
	for (i = 7; i >= 0; --i) {
		if (b & (1 << i)) {
			// send ONE
			sbi_p(portp, pin);
			delay_ns_c(WS_T_1H, -2);
			cbi_p(portp, pin);
			delay_ns_c(WS_T_1L, -10);
		} else {
			// send ZERO
			sbi_p(portp, pin);
			delay_ns_c(WS_T_0H, -2);
			cbi_p(portp, pin);
			delay_ns_c(WS_T_0L, -10);
		}
	}
}
