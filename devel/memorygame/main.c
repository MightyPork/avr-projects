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

// number of cards
#define CARD_COUNT (BOARD_WIDTH * BOARD_HEIGHT)

// number of pairs
#define PAIR_COUNT (CARD_COUNT / 2)

// color palette
const xrgb_t COLORS[] = {
	rgb24_xrgbc(0x00FF99), // emerald
	rgb24_xrgbc(0x00CCCC), // cyan
	rgb24_xrgbc(0x0000CC), // full blue
	rgb24_xrgbc(0xFF6D55), // salmon?
	rgb24_xrgbc(0x4400FF), // blue-purple
	rgb24_xrgbc(0xFF00FF), // magenta
	rgb24_xrgbc(0xD70053), // wine
	rgb24_xrgbc(0xFF0000), // red
	rgb24_xrgbc(0xCD2B64), // brick
	rgb24_xrgbc(0xED1B24), // firetruck red
	rgb24_xrgbc(0xFF6D00), // tangerine yellow/orange
	rgb24_xrgbc(0xFF2B00), // orange
	rgb24_xrgbc(0xFFFF00), // yellow
	rgb24_xrgbc(0x5FBA00), // yellow-green
	rgb24_xrgbc(0x0BEE00), // green
};


// assert valid board size
#if CARD_COUNT % 2 == 1
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
void deal_cards();


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

	deal_cards();

	sei();
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
tile_t board[CARD_COUNT];


void deal_cards()
{
	// clear the board
	for (uint8_t i = 0; i < CARD_COUNT; ++i) {
		board[i] = (tile_t) { .color = 0, .state = GONE };
	}

	// for all pair_COUNT
	for (uint8_t i = 0; i < PAIR_COUNT; ++i) {
		// for both cards in pair
		for (uint8_t j = 0; j < 2; j++) {
			// loop until empty slot is found
			while(1) {
				uint8_t pos = rand() % CARD_COUNT;

				if (board[pos].state == GONE) {
					board[pos] = (tile_t) { .color = i, .state = SECRET };
					break;
				}
			}
		}
	}
}


/** timer 0 interrupt vector */
ISR(TIMER0_COMPA_vect)
{
	debo_tick();  // poll debouncer
	update();  // update game state
	render();
}


// player cursor position
uint8_t cursor = 0;
uint8_t animframe = 0;

bool hide_timeout_match;
uint8_t hide_timeout = 0;

// Game state
uint8_t tiles_revealed = 0;
uint8_t tile1;
uint8_t tile2;

// length of pulse animation (in 10ms)
#define F_ANIM_LEN 20
#define HIDE_TIME 100

// length of button holding before it's repeated (in 10ms)
#define BTNHOLD_REPEAT 20

uint8_t btn_hold_cnt[DEBO_CHANNELS];

void button_click(uint8_t n)
{
	switch (n) {
		case D_UP:
			if (cursor < BOARD_WIDTH)  // first row
				cursor += (CARD_COUNT - BOARD_WIDTH);
			else
				cursor -= BOARD_WIDTH;
			break;

		case D_DOWN:
			if (cursor >= (CARD_COUNT - BOARD_WIDTH))  // last row
				cursor -= (CARD_COUNT - BOARD_WIDTH);
			else
				cursor += BOARD_WIDTH;
			break;

		case D_LEFT:
			if (cursor > 0)  // last row
				cursor--;
			else
				cursor = (CARD_COUNT - 1);
			break;

		case D_RIGHT:
			if (cursor < (CARD_COUNT - 1))  // last row
				cursor++;
			else
				cursor = 0;
			break;

		case D_SELECT:
			if (tiles_revealed == 2) break;  // two already shown
			if (board[cursor].state != SECRET) break;  // selected tile not secret

			// reveal a tile
			if (tiles_revealed < 2) {
				board[cursor].state = REVEALED;
				tiles_revealed++;

				if(tiles_revealed == 1) {
					tile1 = cursor;
				} else {
					tile2 = cursor;
				}
			}

			// Check equality if it's the second
			if (tiles_revealed == 2) {
				hide_timeout_match = (board[tile1].color == board[tile2].color);
				hide_timeout = HIDE_TIME;
			}

			break;

		case D_RESTART:
			deal_cards();
			break;
	}
}


/** Press arrow key, skip empty tiles */
void safe_press_arrow_key(uint8_t n)
{
	// attempt to arrive at some secret tile
	for (uint8_t j = 0; j < BOARD_HEIGHT; j++) {

		for (uint8_t k = 0; k < BOARD_WIDTH; k++) {
			button_click(n);
			if (board[cursor].state != GONE) break;
		}

		if (board[cursor].state != GONE) break;

		// traverse right since current column is empty
		//
		button_click(D_RIGHT);
	}
}


#define is_arrow_key(id) ((id) == D_LEFT || (id) == D_RIGHT || (id) == D_UP || (id) == D_DOWN)


/** Update game (every 10 ms) */
void update()
{
	// handle buttons (repeating)
	for (uint8_t i = 0; i < DEBO_CHANNELS; i++) {
		if (debo_get_pin(i)) {
			if (btn_hold_cnt[i] == 0) {
				if (is_arrow_key(i)) {
					safe_press_arrow_key(i);
				} else {
					button_click(i);
				}
			}

			// non-arrows wrap to 1 -> do not generate repeated clicks
			inc_wrap(btn_hold_cnt[i], is_arrow_key(i) ? 1 : 0, BTNHOLD_REPEAT);

		} else {
			btn_hold_cnt[i] = 0;
		}
	}

	// handle game logic
	if (hide_timeout > 0) {
		if (--hide_timeout == 0) {
			if (hide_timeout_match) {
				// Tiles removed from board
				board[tile1].state = GONE;
				board[tile2].state = GONE;

				if (board[cursor].state == GONE) {
					// move to some other tile
					// try not to change row if possible
					if ((cursor % BOARD_WIDTH) == (BOARD_WIDTH-1))
						safe_press_arrow_key(D_LEFT);
					else
						safe_press_arrow_key(D_RIGHT);
				}
			} else {
				// Tiles made secret again
				board[tile1].state = SECRET;
				board[tile2].state = SECRET;
			}

			tiles_revealed = 0; // no revealed
		}
	}


	inc_wrap(animframe, 0, F_ANIM_LEN * 2);
}

// LED off
#define BLACK rgb24_xrgb(0x000000)
// LED on - secret tile
#define WHITE rgb24_xrgb(0x555555)

// colors to be displayed
xrgb_t screen[CARD_COUNT];


/** Update screen[] and send to display */
void render()
{
	for (uint8_t i = 0; i < CARD_COUNT; i++) {
		switch (board[i].state) {
			case SECRET:
				screen[i] = WHITE;
				break;

			case REVEALED:
				screen[i] = COLORS[board[i].color];
				break;

			default:
			case GONE:
				screen[i] = BLACK;
				break;
		}

		if (i == cursor) {
			// flashy animation state
			uint16_t mult;

			if (animframe < F_ANIM_LEN) {
				mult = animframe;
			} else {
				mult = (F_ANIM_LEN * 2) - animframe;
			}

			screen[i] = (xrgb_t) {
				.r = (uint8_t) ((((uint16_t) screen[i].r) * mult) / F_ANIM_LEN),
				.g = (uint8_t) ((((uint16_t) screen[i].g) * mult) / F_ANIM_LEN),
				.b = (uint8_t) ((((uint16_t) screen[i].b) * mult) / F_ANIM_LEN),
			};
		}
	}

	// debo_get_pin(BTN_LEFT_D) ? PINK : BLACK;

	ws_send_xrgb_array(WS1, screen, CARD_COUNT);
	ws_show();
}


void main()
{
	while(1);  // Timer does everything
}
