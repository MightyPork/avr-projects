#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/cpufunc.h>

#include <util/delay.h>
#include <util/delay_basic.h>

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "arduino_pins.h"
#include "utils.h"
#include "config.h"
#include "ws_driver.h"


void init_io(void) SECTION(".init8");
void init_io()
{
	set_bit(WS_DDR, WS_BIT, OUT);
}


void main()
{
	const uint8_t anim_step = 5;
	const uint8_t anim_max = 255;

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;

	uint8_t step = 0;

	uint8_t r2 = anim_max;
	uint8_t g2 = 0;
	uint8_t b2 = 0;

	uint8_t step2 = 0;


	while (1) {
		r = r2;
		g = g2;
		b = b2;
		step = step2;

		for (uint8_t i = 0; i < 72; i++) {
			ws_send_rgb(r, g, b);

			if (i == 1) {
				r2 = r;
				g2 = g;
				b2 = b;
				step2 = step;
			}

			switch (step) {
				case 0:
					g += anim_step;
					if (g == anim_max) step++;
					break;

				case 1:
					r -= anim_step;
					if (r == 0) step++;
					break;

				case 2:
					b += anim_step;
					if (b == anim_max) step++;
					break;

				case 3:
					g -= anim_step;
					if (g == 0) step++;
					break;

				case 4:
					r += anim_step;
					if (r == anim_max) step++;
					break;

				default:
					b -= anim_step;
					if (b == 0) step = 0;
					break;
			}
		}

		ws_show();
		_delay_ms(30);
	}
}



// void main()
// {
// 	while (1) {
// 		uint8_t r = 250;
// 		uint8_t g = 0;
// 		uint8_t b = 0;

// 		uint8_t step = 0;

// 		for (uint8_t i = 0; i < 30; i++) {
// 			ws_send_rgb(r, g, b);

// 			switch (step) {
// 				case 0:
// 					r -= 50;
// 					g += 50;
// 					if (g == 250) step++;
// 					break;
// 				case 1:
// 					g -= 50;
// 					b += 50;
// 					if (b == 250) step++;
// 					break;
// 				case 2:
// 					b -= 50;
// 					r += 50;
// 					if (r == 250) step=0;
// 					break;
// 			}
// 		}

// 		ws_show();
// 		_delay_ms(100);
// 	}
// }

