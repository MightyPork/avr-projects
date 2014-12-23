#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay_basic.h>
#include <stdint.h>

#include "ws_driver3.h"

#define WS_PORT PORTB
#define WS_BIT 0

uint16_t ws_port = _SFR_ADDR(PORTB);
uint8_t ws_bit;

/** Raise the bus line *///_SFR_MEM8(ws_port)
#define ws_high() do { _SFR_MEM8(ws_port) |= (1 << 0); } while(0)
//#define ws_high() (_SFR_MEM8(_SFR_ADDR(PORTB))) |= (1 << WS_BIT)

/** Drop the bus line */
#define ws_low() do { _SFR_MEM8(ws_port) &= ~(1 << 0); } while(0)
//#define ws_low() (_SFR_MEM8(_SFR_ADDR(PORTB))) &= ~(1 << WS_BIT)

/* Convert nanoseconds to cycle count */
#define ns2cycles(ns)  ( (ns) / (1000000000L / (signed long) F_CPU) )

/** Wait c cycles */
#define delay_c(c)  (((c) > 0) ? __builtin_avr_delay_cycles(c) :  __builtin_avr_delay_cycles(0))

/** Wait n nanoseconds, plus c cycles  */
#define delay_ns_c(ns, c)  delay_c(ns2cycles(ns) + (c))


void ws_bind(uint8_t port_addr, uint8_t bit)
{
	// ws_port = port_addr;
	// ws_bit = bit;
}



/** Latch and display the RGB values */
void ws_show()
{
	ws_low();
	delay_ns_c(WS_T_LATCH, 0);
}

#define _do_ws_send_byte(x) do {								\
	for (int8_t i = 7; i >= 0; --i) {							\
		if (x & (1 << i)) {										\
			ws_high();											\
			delay_ns_c(WS_T_1H, -2);							\
			ws_low();											\
			delay_ns_c(WS_T_1L, -10); 							\
		} else {												\
			ws_high();											\
			delay_ns_c(WS_T_0H, -2);							\
			ws_low();											\
			delay_ns_c(WS_T_0L, -10);							\
		}														\
	}															\
} while(0)

void ws_send_byte(uint8_t b)
{
	_do_ws_send_byte(b);
}


/** Send RGB color to the strip */
void ws_send_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	_do_ws_send_byte(g);
	_do_ws_send_byte(r);
	_do_ws_send_byte(b);
}
