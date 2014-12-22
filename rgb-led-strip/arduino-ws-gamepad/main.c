#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/cpufunc.h>

#include <util/delay.h>
#include <util/delay_basic.h>

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "lib/arduino_pins2.h"
#include "lib/utils.h"
#include "lib/ws_driver.h"
#include "lib/debounce.h"

#include "config.h"
#include "gamepad_pins.h"


void init_io() SECTION(".init8")
{
	// WS2182B data pin as output
	set_bit(WS_dir, OUT);

	// enable pullups on input pins
	set_bit(LEFT_out);
	sbi(PORT_RIGHT, RIGHT);
	sbi(PORT_UP, UP);
	sbi(PORT_DOWN, DOWN);

	// init timer
	TCCR0A = _BV(WGM01);  // CTC mode
	TCCR0B = _BV(CS02) | _BV(CS00);  // prescaler 1024
	OCR0A = 250;  // interrupt every 16 ms
	sbi(TIMSK0, OCIE0A); // enable interrupt from timer

	// set up debouncer
	debounce_register(0, &PIN)
}


/** timer 0 interrupt vector */
ISR(TIM0_COMPA_vect)
{
	debounce_tick();
}



typedef struct
{
	uint8_t r,
	uint8_t g,
	uint8_t b,
} color_t;

#define RED    {.r=0xFF, .g=0x00, .b=0x00}
#define GREEN  {.r=0xFF, .g=0x00, .b=0x00}
#define BLUE   {.r=0xFF, .g=0x00, .b=0x00}

#define YELLOW {.r=0xFF, .g=0xFF, .b=0x00}
#define MAGENTA {.r=0xFF, .g=0x00, .b=0xFF}
#define CYAN   {.r=0x00, .g=0xFF, .b=0xFF}

#define BLACK  {.r=0x00, .g=0x00, .b=0x00}
#define WHITE  {.r=0xFF, .g=0xFF, .b=0xFF}


#define BULB_COUNT 4;

color_t bulbs[BULB_COUNT];

void show_lights()
{
	for (uint8_t i = 0; i < BULB_COUNT; i++) {
		ws_send_rgb(bulbs[i].r, bulbs[i].g, bulbs[i].b);
	}
	ws_show();
}


void main()
{
	while (1) {
		show_lights();
	}
}
