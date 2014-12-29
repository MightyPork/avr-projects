#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "lib/meta.h"
#include "lib/arduino_pins.h"
#include "lib/calc.h"

#define LCD_PIN_RS D10
#define LCD_PIN_RW D11
#define LCD_PIN_E  D12
#define LCD_PIN_D4 D13
#define LCD_PIN_D5 D14
#define LCD_PIN_D6 D15
#define LCD_PIN_D7 D16
// D17 = A3 = source of entropy for random.

#include "lib/hd44780.h"


// Buttons (to ground)
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

#define DEBO_CHANNELS 6
#define DEBO_TICKS 1  // in 0.01s

#include "lib/debounce.h"

// Board size (!!! rows = 2x number of display lines, max 2*4 = 8 !!!)
#define ROWS 4
#define COLS 20

// proto
void render();
void update();


void SECTION(".init8") init()
{
	// Randomize RNG
	adc_init();
	srand(adc_read_word(3));

	// Init LCD
	lcd_init();

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
	debo_tick();  // poll debouncer
	update();  // update game state
	render();
}



// sub-glyphs
#define _HEAD_ 31, 21, 21, 31
#define _BODY_ 31, 31, 31, 31
#define _FOOD_ 10, 21, 17, 14
#define _NONE_  0,  0,  0,  0

// complete glyphs for loading into memory

// Only one food & one head glyph have to be loaded at a time.

// Body - Body
const uint8_t BB[] PROGMEM = {_BODY_, _BODY_};

// Body - None
const uint8_t BX[] PROGMEM = {_BODY_, _NONE_};
const uint8_t XB[] PROGMEM = {_NONE_, _BODY_};

// Head - None
const uint8_t HX[] PROGMEM = {_HEAD_, _NONE_};
const uint8_t XH[] PROGMEM = {_NONE_, _HEAD_};

// Body - Head
const uint8_t BH[] PROGMEM = {_BODY_, _HEAD_};
const uint8_t HB[] PROGMEM = {_HEAD_, _BODY_};

// Head - Food
const uint8_t HF[] PROGMEM = {_HEAD_, _FOOD_};
const uint8_t FH[] PROGMEM = {_FOOD_, _HEAD_};

// Food - None
const uint8_t FX[] PROGMEM = {_FOOD_, _NONE_};
const uint8_t XF[] PROGMEM = {_NONE_, _FOOD_};

// Body - Food
const uint8_t BF[] PROGMEM = {_BODY_, _FOOD_};
const uint8_t FB[] PROGMEM = {_FOOD_, _BODY_};

// board block (snake, food...)
typedef enum {
	EMPTY = 0x00,
	HEAD = 0x01,
	FOOD = 0x02,
	BODY_LEFT = 0x80;
	BODY_RIGHT = 0x81,
	BODY_UP = 0x82,
	BODY_DOWN = 0x83,
} block_t;

typedef struct {
	uint8_t x;
	uint8_t y;
} coord_t;

#define is_body(blk) ((blk) & 0x80)

// y, x indexing
block_t board[ROWS][COLS];

coord_t head_pos;
coord_t food_pos;


void main()
{
	lcd_define_glyph_pgm(0, BX);
	lcd_define_glyph_pgm(1, BB);
	lcd_define_glyph_pgm(2, XB);
	lcd_define_glyph_pgm(3, XH);
	lcd_define_glyph_pgm(4, XF);

	// Test
	lcd_char_xy(5, 0, 0);
	lcd_char_xy(6, 0, 0);
	lcd_char_xy(7, 0, 1);
	lcd_char_xy(7, 1, 0);
	lcd_char_xy(8, 1, 0);
	lcd_char_xy(9, 1, 1);
	lcd_char_xy(10, 1, 2);
	lcd_char_xy(11, 1, 3);
	lcd_char_xy(14, 1, 4);

	while(1);
}
