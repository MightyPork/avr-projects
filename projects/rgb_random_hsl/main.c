#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>

#include "lib/meta.h"
#include "lib/arduino_pins.h"
#include "lib/calc.h"

#include "lib/colors.h"
#include "lib/hsl.h"
#include "lib/adc.h"

#define WS_T_1H  800
#define WS_T_1L  400
#define WS_T_0H  120
#define WS_T_0L  900

#include "lib/ws_rgb.h"

#define WS1   D2

void SECTION(".init8") init()
{
	adc_init();
	srand(adc_read_word(0));

	as_output(WS1);
}


void main()
{
	#define SIZE 16
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

			screen[i] = hsl2xrgb(board[i]);
		}

		if (rand() % 50 == 0) {
			uint8_t i = rand() % SIZE;

			if (board[i].l == 0) {
				board[i].h = rand() % 256;
				board[i].s = 200 + rand() % 56;
				board[i].l = 255;
			}
		}

		ws_send_xrgb_array(WS1, screen, SIZE);
		_delay_ms(2);
	}
}
