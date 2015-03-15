#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
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


typedef struct {
	uint8_t h;
	uint8_t s;
	uint8_t l;
} hsl_t;


// based on: https://github.com/lewisd32/avr-hsl2rgb
xrgb_t hsl2rgb(const hsl_t cc)
{
	// 0 .. 256*3
	const uint16_t hh = (uint16_t) cc.h * 3;
	const uint8_t hue_mod = hh % 256;

	uint8_t r_temp, g_temp, b_temp;
	if (hh < 256) {
		r_temp = hue_mod ^ 255;
		g_temp = hue_mod;
		b_temp = 0;
	} else if (hh < 512) {
		r_temp = 0;
		g_temp = hue_mod ^ 255;
		b_temp = hue_mod;
	} else if (hh < 768) {
		r_temp = hue_mod;
		g_temp = 0;
		b_temp = hue_mod ^ 255;
	} else {
		r_temp = 0;
		g_temp = 0;
		b_temp = 0;
	}

	const uint8_t inverse_sat = (cc.s ^ 255);

	xrgb_t rgb;

	uint8_t t8;
	uint16_t t16;

	t8 = r_temp;
	t16 = t8 * cc.s + t8;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	t8 = t8 + inverse_sat;
	t16 = t8 * cc.l;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	rgb.r = t8;

	t8 = g_temp;
	t16 = t8 * cc.s;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	t8 = t8 + inverse_sat;
	t16 = t8 * cc.l;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	rgb.g = t8;

	t8 = b_temp;
	t16 = t8 * cc.s;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	t8 = t8 + inverse_sat;
	t16 = t8 * cc.l;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	rgb.b = t8;

	return rgb;
}


void SECTION(".init8") init()
{
	adc_init();
	srand(adc_read_word(0));

	as_output(WS1);
}


void main()
{
	#define SIZE 7
	hsl_t board[SIZE];
	xrgb_t screen[SIZE];

	for (uint8_t i = 0; i < SIZE; i++) {
		board[i]  = (hsl_t)  {.h=0, .s=255, .l=0};
		screen[i] = (xrgb_t) {.r=0, .g=0, .b=0};
	}

	while(1) {
		for(uint8_t i = 0; i < SIZE; i++) {
			if (board[i].l > 0) {
				board[i].l--;
			}

			screen[i] = hsl2rgb(board[i]);
		}

		if (rand() % 200 == 0) {
			uint8_t i = rand() % SIZE;

			if (board[i].l == 0) {
				board[i].h = rand() % 256;
				board[i].s = 200 + rand() % 56;
				board[i].l = 255;
			}
		}

		ws_send_xrgb_array(WS1, screen, SIZE);
		_delay_ms(10);
	}
}
