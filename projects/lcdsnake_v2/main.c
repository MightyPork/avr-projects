#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "lib/arduino_pins.h"
#include "lib/calc.h"
#include "lib/adc.h"
#include "lib/lcd.h"
#include "lib/debounce.h"

// Buttons (to ground)
#define BTN_LEFT     A0
#define BTN_RIGHT    A1
#define BTN_UP       A2
#define BTN_DOWN     A3
#define BTN_PAUSE    A4
#define BTN_RESTART  A5

// Debouncer channels for buttons
// (Must be added in this order to debouncer)
#define D_LEFT      0
#define D_RIGHT     1
#define D_UP        2
#define D_DOWN      3
#define D_PAUSE     4
#define D_RESTART   5

// Board size (!!! rows = 2x number of display lines, max 2*4 = 8 !!!)
#define ROWS 8
#define COLS 20

// Delay between snake steps, in 10 ms
#define STEP_DELAY 24

// proto
void update();
void init_cgram();
void init_gameboard();

void init()
{
	// Randomize RNG
	adc_init();
	srand(adc_read_word(3));

	// Init LCD
	lcd_init();
	init_cgram();  // load default glyphs

	// Init game board.
	init_gameboard();

	// gamepad buttons
	as_input_pu(BTN_LEFT);
	as_input_pu(BTN_RIGHT);
	as_input_pu(BTN_UP);
	as_input_pu(BTN_DOWN);
	as_input_pu(BTN_PAUSE);
	as_input_pu(BTN_RESTART);

	// add buttons to debouncer
	debo_add_rev(BTN_LEFT);
	debo_add_rev(BTN_RIGHT);
	debo_add_rev(BTN_UP);
	debo_add_rev(BTN_DOWN);
	debo_add_rev(BTN_PAUSE);
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
	debo_tick();  // poll debouncer
	update();  // update and display
}



// sub-glyphs
#define _HEAD_ 15, 21, 21, 30
#define _BODY_ 15, 31, 31, 30
#define _FOOD_ 10, 21, 17, 14
//14, 17, 17, 14
#define _NONE_  0,  0,  0,  0

// complete glyphs for loading into memory

// Only one food & one head glyph have to be loaded at a time.

// Body - Body
const uint8_t SYMBOL_BB[] PROGMEM = {_BODY_, _BODY_};

// Body - None
const uint8_t SYMBOL_BX[] PROGMEM = {_BODY_, _NONE_};
const uint8_t SYMBOL_XB[] PROGMEM = {_NONE_, _BODY_};

// Head - None
const uint8_t SYMBOL_HX[] PROGMEM = {_HEAD_, _NONE_};
const uint8_t SYMBOL_XH[] PROGMEM = {_NONE_, _HEAD_};

// Body - Head
const uint8_t SYMBOL_BH[] PROGMEM = {_BODY_, _HEAD_};
const uint8_t SYMBOL_HB[] PROGMEM = {_HEAD_, _BODY_};

// Head - Food
const uint8_t SYMBOL_HF[] PROGMEM = {_HEAD_, _FOOD_};
const uint8_t SYMBOL_FH[] PROGMEM = {_FOOD_, _HEAD_};

// Food - None
const uint8_t SYMBOL_FX[] PROGMEM = {_FOOD_, _NONE_};
const uint8_t SYMBOL_XF[] PROGMEM = {_NONE_, _FOOD_};

// Body - Food
const uint8_t SYMBOL_BF[] PROGMEM = {_BODY_, _FOOD_};
const uint8_t SYMBOL_FB[] PROGMEM = {_FOOD_, _BODY_};


// board block (snake, food...)
typedef enum {
	bEMPTY = 0x00,
	bHEAD = 0x01,
	bFOOD = 0x02,
	bBODY_LEFT = 0x80,
	bBODY_RIGHT = 0x81,
	bBODY_UP = 0x82,
	bBODY_DOWN = 0x83,
} block_t;

// Snake direction
typedef enum {
	dLEFT = 0x00,
	dRIGHT = 0x01,
	dUP = 0x02,
	dDOWN = 0x03,
} dir_t;

// Coordinate on board
typedef struct {
	int8_t x;
	int8_t y;
} coord_t;

#define is_body(blk) (((blk) & 0x80) != 0)
#define mk_body_dir(dir) (0x80 + (dir))

// compare two coords
#define coord_eq(a, b) (((a).x == (b).x) && ((a).y == (b).y))


bool crashed;
uint8_t snake_len;

// y, x indexing
block_t board[ROWS][COLS];

coord_t head_pos;
coord_t tail_pos;
dir_t head_dir;

const uint8_t CODE_BB = 0;
const uint8_t CODE_BX = 1;
const uint8_t CODE_XB = 2;
const uint8_t CODE_H = 3;  // glyph with head, dynamic
const uint8_t CODE_F = 4;  // glyph with food, dynamic
const uint8_t CODE_XX = 32; // space


// Set a block in board
#define set_block_xy(x, y, block) do { board[y][x] = (block); } while(0)
#define get_block_xy(x, y) board[y][x]
#define get_block(pos) get_block_xy((pos).x, (pos).y)
#define set_block(pos, block) set_block_xy((pos).x, (pos).y, (block))


void init_cgram()
{
	// those will be always the same
	lcd_glyph_P(CODE_BB, SYMBOL_BB);
	lcd_glyph_P(CODE_BX, SYMBOL_BX);
	lcd_glyph_P(CODE_XB, SYMBOL_XB);
						lcd_glyph_P(5, SYMBOL_XF);
}


void place_food()
{
	while(1) {
		const uint8_t xx = rand() % COLS;
		const uint8_t yy = rand() % ROWS;

		if (get_block_xy(xx, yy) == bEMPTY) {
			set_block_xy(xx, yy, bFOOD);
			break;
		}
	}
}


void init_gameboard()
{
	//erase the board
	for (uint8_t x = 0; x < COLS; x++) {
		for (uint8_t y = 0; y < ROWS; y++) {
			set_block_xy(x, y, bEMPTY);
		}
	}

	lcd_clear();

	tail_pos = (coord_t) {.x = 0, .y = 0};

	set_block_xy(0, 0, bBODY_RIGHT);
	set_block_xy(1, 0, bBODY_RIGHT);
	set_block_xy(2, 0, bBODY_RIGHT);
	set_block_xy(3, 0, bHEAD);

	head_pos = (coord_t) {.x = 3, .y = 0};

	snake_len = 4; // includes head

	head_dir = dRIGHT;
	crashed = false;

	place_food();
}


uint8_t presc = 0;

bool restart_held;
bool pause_held;
bool paused;
void update()
{
	if (debo_get_pin(D_RESTART)) {

		if (!restart_held) {
			// restart
			init_gameboard();
			presc = 0;
			restart_held = true;
		}

	} else {
		restart_held = false;
	}

	if (debo_get_pin(D_PAUSE)) {

		if (!pause_held) {
			paused ^= true;
			pause_held = true;
		}

	} else {
		pause_held = false;
	}

	if(!crashed && !paused) {

		// resolve movement direction
		if (debo_get_pin(D_LEFT))
			head_dir = dLEFT;
		else if (debo_get_pin(D_RIGHT))
			head_dir = dRIGHT;
		else if (debo_get_pin(D_UP))
			head_dir = dUP;
		else if (debo_get_pin(D_DOWN))
			head_dir = dDOWN;

		// time's up for a move
		if (presc++ == STEP_DELAY) {
			presc = 0;

			// move snake
			const coord_t oldpos = head_pos;

			switch (head_dir) {
				case dLEFT:  head_pos.x--; break;
				case dRIGHT: head_pos.x++; break;
				case dUP:    head_pos.y--; break;
				case dDOWN:  head_pos.y++; break;
			}

			bool do_move = false;
			bool do_grow = false;

			if (head_pos.x < 0 || head_pos.x >= COLS || head_pos.y < 0 || head_pos.y >= ROWS) {
				// ouch, a wall!
				crashed = true;
			} else {
				// check if tile occupied or not
				if (coord_eq(head_pos, tail_pos)) {
					// head moved in previous tail, that's OK.
					do_move = true;
				} else {
					// moved to other tile than tail
					switch (get_block(head_pos)) {

						case bFOOD:
							do_grow = true;  // fall through
						case bEMPTY:
							do_move = true;
							break;

						default: // includes all BODY_xxx
							crashed = true;  // snake crashed into some block
					}
				}
			}

			if (do_move) {
				// Move tail
				if (do_grow) {
					// let tail as is
					snake_len++; // grow the counter
				} else {
					// tail dir
					dir_t td = get_block(tail_pos) & 0xF;

					// clean tail
					set_block(tail_pos, bEMPTY);

					// move tail based on old direction of tail block
					switch (td) {
						case dLEFT:  tail_pos.x--; break;
						case dRIGHT: tail_pos.x++; break;
						case dUP:    tail_pos.y--; break;
						case dDOWN:  tail_pos.y++; break;
					}
				}

				// Move head
				set_block(head_pos, bHEAD);  // place head in new pos
				set_block(oldpos, mk_body_dir(head_dir));  // directional body in old head pos

				if (do_grow) {
					// food eaten, place new
					place_food();
				}
			}
		}
	} // end !crashed


	// Render the board
	for (uint8_t r = 0; r < ROWS / 2; r++)	{
		lcd_xy(0, r);
		for (uint8_t c = 0; c < COLS; c++)	{
			const block_t t1 = get_block_xy(c, r * 2);
			const block_t t2 = get_block_xy(c, (r * 2) + 1);

			uint8_t code = '!';  // ! marks fail

			if ((t1 == bEMPTY) && (t2 == bEMPTY)) {
				code = CODE_XX;
				if (crashed) code = '*';
			} else if (is_body(t1) && is_body(t2))
				code = CODE_BB;
			else if (is_body(t1) && (t2 == bEMPTY))
				code = CODE_BX;
			else if (t1 == bEMPTY && is_body(t2))
				code = CODE_XB;
			else if ((t1 == bFOOD) || (t2 == bFOOD)) {
				// one is food

				code = CODE_F;

				if (t1 == bFOOD) {
					if (t2 == bEMPTY)
						lcd_glyph_P(code, SYMBOL_FX);
					else if (t2 == bHEAD)
						lcd_glyph_P(code, SYMBOL_FH);
					else if (is_body(t2))
						lcd_glyph_P(code, SYMBOL_FB);
				} else { // t2 is food
					if (t1 == bEMPTY)
						lcd_glyph_P(code, SYMBOL_XF);
					else if (t1 == bHEAD)
						lcd_glyph_P(code, SYMBOL_HF);
					else if (is_body(t1))
						lcd_glyph_P(code, SYMBOL_BF);
				}
				lcd_xy(c,r);

			} else if ((t1 == bHEAD )|| (t2 == bHEAD)) {
				// one is head

				code = CODE_H;

				if (t1 == bHEAD) {
					if (t2 == bEMPTY)
						lcd_glyph_P(code, SYMBOL_HX);
					else if (is_body(t2))
						lcd_glyph_P(code, SYMBOL_HB);
				} else { // t2 is head
					if (t1 == bEMPTY)
						lcd_glyph_P(code, SYMBOL_XH);
					else if (is_body(t1))
						lcd_glyph_P(code, SYMBOL_BH);
				}

				lcd_xy(c,r);
			}

			lcd_putc(code);
		}
	}
}

void main()
{
	init();

	while(1); // timer does everything
}
