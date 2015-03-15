#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// #include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "lib/meta.h"
#include "lib/arduino_pins.h"
#include "lib/calc.h"
#include "lib/colors.h"
#include "lib/adc.h"

#define WS_T_1H  800
#define WS_T_1L  400
#define WS_T_0H  120
#define WS_T_0L  900

#include "lib/ws_rgb.h"

#define WS1   D2


void SECTION(".init8") init()
{
	as_output(WS1);
}

void main()
{
	const uint8_t anim_step = 50;
	const uint8_t anim_max = 250;
	const uint8_t pixel_count = 7;

	xrgb_t color = xrgb(anim_max, 0, 0);
	uint8_t step = 0;

	xrgb_t color2 = xrgb(anim_max, 0, 0);
	uint8_t step2 = 0;

	while (1) {

		color = color2;
		step = step2;

		for (uint8_t i = 0; i < pixel_count; i++) {
			ws_send_xrgb(WS1, color);

			if (i == 1) {
				color2 = color;
				step2 = step;
			}

			switch (step) {
				case 0:
					color.g += anim_step;
					if (color.g >= anim_max) step++;
					break;
				case 1:
					color.r -= anim_step;
					if (color.r == 0) step++;
					break;
				case 2:
					color.b += anim_step;
					if (color.b >= anim_max) step++;
					break;
				case 3:
					color.g -= anim_step;
					if (color.g == 0) step++;
					break;
				case 4:
					color.r += anim_step;
					if (color.r >= anim_max) step++;
					break;
				default:
					color.b -= anim_step;
					if (color.b == 0) step = 0;
					break;
			}
		}

		ws_show();

		_delay_ms(100);
	}
}
