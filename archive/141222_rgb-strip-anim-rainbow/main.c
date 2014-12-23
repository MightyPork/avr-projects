#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>

#include "lib/meta.h"
#include "lib/arduino_pins.h"
#include "lib/calc.h"
#include "lib/colors.h"
#include "lib/ws2812.h"

#define WS1 D8
#define WS2 D9

void SECTION(".init8") init_io()
{
	as_output(WS1);
	as_output(WS2);
}

void main()
{
	const uint8_t anim_step = 10;
	const uint8_t anim_max = 250;
	const uint8_t pixel_count = 30;

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

		_delay_ms(20);
	}
}
