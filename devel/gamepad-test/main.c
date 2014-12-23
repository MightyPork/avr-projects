#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// #include <stdbool.h>
#include <stdint.h>

#include "lib/meta.h"
#include "lib/arduino_pins.h"
#include "lib/calc.h"
#include "lib/colors.h"
#include "lib/ws2812.h"

#define DEBO_CHANNELS 11
#define DEBO_TICKS 1  // 0.01s

#include "lib/debounce.h"

#define WS1   D10

#define BTN_LEFT  D2
#define BTN_RIGHT D3
#define BTN_UP    D4
#define BTN_DOWN  D5
#define BTN_A     D6
#define BTN_B     D7
#define BTN_AA    D8
#define BTN_BB    D9
#define BTN_SLOW   D14
#define BTN_START  D15
#define BTN_SELECT D16

void render();

void SECTION(".init8") init_io()
{
	// led strip data
	as_output(WS1);

	// gamepad buttons
	as_input_pu(BTN_LEFT);
	as_input_pu(BTN_RIGHT);
	as_input_pu(BTN_UP);
	as_input_pu(BTN_DOWN);

	as_input_pu(BTN_A);
	as_input_pu(BTN_B);
	as_input_pu(BTN_AA);
	as_input_pu(BTN_BB);

	as_input_pu(BTN_SLOW);
	as_input_pu(BTN_START);
	as_input_pu(BTN_SELECT);

	// set up debouncers
	#define BTN_LEFT_D 0
	debo_add_rev(BTN_LEFT);

	#define BTN_RIGHT_D 1
	debo_add_rev(BTN_RIGHT);

	#define BTN_UP_D 2
	debo_add_rev(BTN_UP);

	#define BTN_DOWN_D 3
	debo_add_rev(BTN_DOWN);

	#define BTN_A_D 4
	debo_add_rev(BTN_A);

	#define BTN_B_D 5
	debo_add_rev(BTN_B);

	#define BTN_AA_D 6
	debo_add_rev(BTN_AA);

	#define BTN_BB_D 7
	debo_add_rev(BTN_BB);

	#define BTN_SLOW_D 8
	debo_add_rev(BTN_SLOW);

	#define BTN_START_D 9
	debo_add_rev(BTN_START);

	#define BTN_SELECT_D 10
	debo_add_rev(BTN_SELECT);


	// setup timer 10 ms
	TCCR0A = _BV(WGM01); // CTC
	TCCR0B = _BV(CS02) | _BV(CS00);  // prescaler 512
	OCR0A = 156;  // interrupt every 10 ms
	sbi(TIMSK0, OCIE0A);
	sei();
}

/** timer 0 interrupt vector */
ISR(TIMER0_COMPA_vect)
{
	debo_tick();  // poll debouncer
	render();
}


void render()
{
	const rgb24_t PINK = 0x970078;
	const rgb24_t BLUE = 0x005397;
	const rgb24_t RED = 0xFF0000;
	const rgb24_t YELLOW = 0xFFFF00;
	const rgb24_t BLACK = 0x000000;

	static rgb24_t table[12];

	table[0] = debo_get_pin(BTN_LEFT_D) ? PINK : BLACK;
	table[1] = debo_get_pin(BTN_RIGHT_D) ? PINK : BLACK;
	table[2] = debo_get_pin(BTN_UP_D) ? PINK : BLACK;
	table[3] = debo_get_pin(BTN_DOWN_D) ? PINK : BLACK;
	table[4] = debo_get_pin(BTN_A_D) ? BLUE : BLACK;
	table[5] = debo_get_pin(BTN_B_D) ? BLUE : BLACK;
	table[6] = debo_get_pin(BTN_AA_D) ? BLUE : BLACK;
	table[7] = debo_get_pin(BTN_BB_D) ? BLUE : BLACK;
	table[8] = debo_get_pin(BTN_SELECT_D) ? RED : BLACK;
	table[9] = debo_get_pin(BTN_START_D) ? RED : BLACK;
	table[10] = debo_get_pin(BTN_SLOW_D) ? RED : BLACK;
	table[11] = YELLOW;

	rgb24_t c;
	for (uint8_t i=0; i<30; i++) {

		if (i < 12)
			c = table[i];
		else
			c = BLACK;

		ws_send_rgb24(WS1, c);
	}
}


void main()
{
	// render();
	while(1) {}
}
