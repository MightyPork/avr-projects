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
	adc_init();
	srand(adc_read_word(0));

	as_output(WS1);

	cli();
}


typedef struct {
	xrgb_t act;
	xrgb_t target;
} cell_t;


void update_cnt(uint8_t* act, uint8_t* target)
{
	if (*act == *target) {
		//*target = rand() % 256;
		int8_t add = -10 + rand() % 21;

		if(add > 0) {
			if (add + (*target) <= 255) {
				*target += add;
			} else {
				*target = 255;
			}
		} else {
			if(add + (*target) >= 0) {
				*target += add;
			} else {
				*target = 0;
			}
		}
	} else {
		if (*act < *target) {
			(*act)++;
		} else {
			(*act)--;
		}
	}
}

void update_cell(cell_t* cell)
{
	update_cnt(&(cell->act.r), &(cell->target.r));
	update_cnt(&(cell->act.g), &(cell->target.g));
	update_cnt(&(cell->act.b), &(cell->target.b));
}

void main()
{
	#define LEN 16
	cell_t rainbow[LEN];

	for(uint8_t i=0; i<LEN; i++) {
		rainbow[i] = (cell_t){
			.act    = rgb24_xrgbc(0x888888),
			.target = rgb24_xrgbc(0x888888)
		};
	}

	while(1) {
		for(uint8_t i=0; i<LEN; i++) {
			update_cell(&rainbow[i]);
		}

		for(uint8_t i=0; i<LEN; i++) {
			ws_send_xrgb(WS1, rainbow[i].act);
		}

		_delay_ms(10);
	}
}
