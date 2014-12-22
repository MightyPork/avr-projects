#include <avr/io.h>
#include <util/delay_basic.h>
#include <stdint.h>
#include "ws_driver.h"

/** Raise the bus line */
#define ws_high() (WS_PORT) |= (1 << WS_BIT)

/** Drop the bus line */
#define ws_low() (WS_PORT) &= ~(1 << WS_BIT)

/* Convert nanoseconds to cycle count */
#define ns2cycles(ns)  ( (ns) / (1000000000L / (signed long) F_CPU) )

/** Wait c cycles */
#define delay_c(c)  (((c) > 0) ? __builtin_avr_delay_cycles(c) :  __builtin_avr_delay_cycles(0))

/** Wait n nanoseconds, plus c cycles  */
#define delay_ns_c(ns, c)  delay_c(ns2cycles(ns) + (c))


/** Latch and display the RGB values */
void ws_show()
{
	ws_low();
	delay_ns_c(WS_T_LATCH, 0);
}


/** Send one byte to the RGB strip */
void ws_send_byte(uint8_t bb)
{
	for (int8_t i = 7; i >= 0; --i) {
		if (bb & (1 << i)) {
			// send ONE
			ws_high();
			delay_ns_c(WS_T_1H, -2);
			ws_low();
			delay_ns_c(WS_T_1L, -10); // compensation for overhead
		} else {
			// send ZERO
			ws_high();
			delay_ns_c(WS_T_0H, -2);
			ws_low();
			delay_ns_c(WS_T_0L, -10);
		}
	}
}


/** Send RGB color to the strip */
void ws_send_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	int8_t i;

	// manualy inlined, because `inline` doesn't work with delays.

	// GREEN
	for (i = 7; i >= 0; --i) {
		if (g & (1 << i)) {
			// send ONE
			ws_high();
			delay_ns_c(WS_T_1H, -2);
			ws_low();
			delay_ns_c(WS_T_1L, -10);
		} else {
			// send ZERO
			ws_high();
			delay_ns_c(WS_T_0H, -2);
			ws_low();
			delay_ns_c(WS_T_0L, -10);
		}
	}

	// RED
	for (i = 7; i >= 0; --i) {
		if (r & (1 << i)) {
			// send ONE
			ws_high();
			delay_ns_c(WS_T_1H, -2);
			ws_low();
			delay_ns_c(WS_T_1L, -10);
		} else {
			// send ZERO
			ws_high();
			delay_ns_c(WS_T_0H, -2);
			ws_low();
			delay_ns_c(WS_T_0L, -10);
		}
	}

	// BLUE
	for (i = 7; i >= 0; --i) {
		if (b & (1 << i)) {
			// send ONE
			ws_high();
			delay_ns_c(WS_T_1H, -2);
			ws_low();
			delay_ns_c(WS_T_1L, -10);
		} else {
			// send ZERO
			ws_high();
			delay_ns_c(WS_T_0H, -2);
			ws_low();
			delay_ns_c(WS_T_0L, -10);
		}
	}
}
