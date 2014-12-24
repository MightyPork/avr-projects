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
#include "lib/ws2812.h"
#include "lib/adc.h"

#define DEBO_CHANNELS 6
#define DEBO_TICKS 1  // in 0.01s

#include "lib/debounce.h"


#define BOARD_WIDTH 6
#define BOARD_HEIGHT 5
#define BOARD_LEN (BOARD_WIDTH * BOARD_HEIGHT)
#define TILE_COUNT (BOARD_LEN / 2)

const xrgb_t COLORS[] = {
	rgb24_xrgbc(0xB14835), // brick red
	rgb24_xrgbc(0x4C1661), // indigo
	rgb24_xrgbc(0xA3268E), // royal purple
	rgb24_xrgbc(0x009ADA), // cobalt blue
	rgb24_xrgbc(0x007D8F), // teal
	rgb24_xrgbc(0x002E5A), // navy blue
	rgb24_xrgbc(0x56BCC1), // turquoise
	rgb24_xrgbc(0x01654D), // emerald dark
	rgb24_xrgbc(0xF5864F), // papaya orange
	rgb24_xrgbc(0xED1B24), // firetruck red
	rgb24_xrgbc(0xFDAF17), // tangerine orange
	rgb24_xrgbc(0xF58F83), // coral
	rgb24_xrgbc(0xED008C), // magenta
	rgb24_xrgbc(0xFEF200), // sunshine yellow
	rgb24_xrgbc(0x6D9346), // treetop green
};


// assert valid board size
#if BOARD_LEN % 2 == 1
# error "Board size is not even!"
#endif


#define WS1   D10

#define BTN_LEFT     D2
#define BTN_RIGHT    D3
#define BTN_UP       D4
#define BTN_DOWN     D5
#define BTN_SELECT   D6
#define BTN_RESTART  D7

// Debouncer channels for buttons
// (Must be added in this order to debouncer)
#define D_LEFT      0
#define D_RIGHT     1
#define D_UP        2
#define D_DOWN      3
#define D_SELECT    4
#define D_RESTART   5

// Pin A0 not connected to anything, used to get
// entropy for random number generator

// Prototypes
void render();
void update();


void SECTION(".init8") init()
{
	adc_init();  // Initialize ADC
	srand(adc_read_word(0));  // Randomize RNG

	// led strip data
	as_output(WS1);

	// gamepad buttons
	as_input_pu(BTN_LEFT);
	as_input_pu(BTN_RIGHT);
	as_input_pu(BTN_UP);
	as_input_pu(BTN_DOWN);
	as_input_pu(BTN_SELECT);
	as_input_pu(BTN_RESTART);

	// add buttons to debouncer
	debo_add_rev(BTN_LEFT);
	debo_add_rev(BTN_RIGHT);
	debo_add_rev(BTN_UP);
	debo_add_rev(BTN_DOWN);
	debo_add_rev(BTN_SELECT);
	debo_add_rev(BTN_RESTART);

	// setup timer
	TCCR0A = _BV(WGM01); // CTC
	TCCR0B = _BV(CS02) | _BV(CS00);  // prescaler 1024
	OCR0A = 156;  // interrupt every 10 ms
	sbi(TIMSK0, OCIE0A);


	sei();
}


/** timer 0 interrupt vector */
ISR(TIMER0_COMPA_vect)
{
	//debo_tick();  // poll debouncer
	update();  // update game state
	render();
}


void main()
{
	while(1);  // Timer does everything
}


/** Tile state enum */
typedef enum {
	SECRET,
	REVEALED,
	GONE
} tilestate_t;


/** Tile struct */
typedef struct {
	uint8_t color;  // color index from COLORS[]
	tilestate_t state;  // state of the tile (used for render)
} tile_t;


// board tiles
tile_t board[BOARD_LEN];

// player cursor position
uint8_t cursor = 0;

uint8_t noise = 0;
uint8_t xxx = 0;

/** Update game */
void update()
{
	if(xxx++ >= 10) {
		noise = rand();
		xxx = 0;
	}
}


// colors to be displayed
rgb24_t screen[BOARD_LEN];

/** Update screen[] and send to display */
void render()
{
	#define BLUE 0x005397
	#define RED 0xFF0000
	#define YELLOW 0xFFFF00
	#define BLACK 0x000000

	for (uint8_t i = 0; i < BOARD_LEN; i++) {
		if (i < 8) {
			screen[i] = get_bit(noise, i) ? RED : BLUE;
		} else {
			screen[i] = YELLOW;
		}
	}

	// debo_get_pin(BTN_LEFT_D) ? PINK : BLACK;

	ws_send_rgb24_array(WS1, screen, BOARD_LEN);
	ws_show();
}

