#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include <util/delay_basic.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "utils.h"
#include "arduino_pins.h"

#define WS_PORT PORT_D8
#define WS_BIT D8
#define WS_DDR DDR_D8

#define ws_high() sbi(WS_PORT, WS_BIT)
#define ws_low() cbi(WS_PORT, WS_BIT)


#define T1H  800    // Width of a 1 bit in ns
#define T1L  450    // Width of a 1 bit in ns

#define T0H  200    // Width of a 0 bit in ns
#define T0L  650    // Width of a 0 bit in ns

#define RES 50000    // Width of the low gap between bits to cause a frame to latch

#define NS_PER_SEC (1000000000L)          // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives

#define CYCLES_PER_SEC (16000000L)

#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC )

#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )

#define DELAY_CYCLES(n) ( ((n)>0) ? __builtin_avr_delay_cycles( n ) :  __builtin_avr_delay_cycles( 0 ) )  // Make sure we never have a delay less than zero

void ws_show()
{
	ws_low();
	DELAY_CYCLES( NS_TO_CYCLES(RES) );
}


void ws_send_byte(uint8_t bb)
{
	for (int8_t i = 7; i >= 0; --i) {
		if (bb & (1 << i)) {
			ws_high();
			DELAY_CYCLES( NS_TO_CYCLES( T1H ) - 2 );
			ws_low();
			DELAY_CYCLES( NS_TO_CYCLES( T1L ) - 10 );
		} else {
			ws_high();
			DELAY_CYCLES( NS_TO_CYCLES( T0H ) - 2 );
			ws_low();
			DELAY_CYCLES( NS_TO_CYCLES( T0L ) - 10 );
		}
	}
}


void ws_send_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	ws_send_byte(g);
	ws_send_byte(r);
	ws_send_byte(b);
}


void init_io(void) SECTION(".init8");
void init_io()
{
	set_bit(WS_DDR, WS_BIT, OUT);
}


void main()
{
	while (1) {
		uint8_t r = 250;
		uint8_t g = 0;
		uint8_t b = 0;

		uint8_t step = 0;

		for (uint8_t i = 0; i < 30; i++) {
			ws_send_rgb(r, g, b);

			switch (step) {
				case 0:
					r -= 50;
					g += 50;
					if (g == 250) step++;
					break;
				case 1:
					g -= 50;
					b += 50;
					if (b == 250) step++;
					break;
				case 2:
					b -= 50;
					r += 50;
					if (r == 250) step=0;
					break;
			}
		}

		// ws_send_rgb(0x00, 0xFF, 0);
		ws_show();
	}
}
