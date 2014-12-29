#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "arduino_pins.h"
#include "calc.h"
#include "pins.h"
#include "nsdelay.h"

/*
  HD44780 LCD display driver - 4-bit mode

  Required macros - pin settings (eg. B,3 or D,0)

      LCD_PIN_RS
      LCD_PIN_RW
      LCD_PIN_E
      LCD_PIN_D7
      LCD_PIN_D6
      LCD_PIN_D5
      LCD_PIN_D4

  Define those before including the header file.
*/

// Commands for user

// Clear screen (reset)
#define LCD_CLEAR 0b00000001
// Move cursor to (0,0), unshift...
#define LCD_HOME  0b00000010

// Set mode: Increment + NoShift
#define LCD_MODE_INC         0b00000110
// Set mode: Increment + Shift
#define LCD_MODE_INC_SHIFT   0b00000111

// Set mode: Decrement + NoShift
#define LCD_MODE_DEC         0b00000100
// Set mode: Decrement + Shift
#define LCD_MODE_DEC_SHIFT   0b00000101

// Disable display (data remains untouched)
#define LCD_DISABLE          0b00001000

// Disable cursor
#define LCD_CURSOR_NONE      0b00001100
// Set cursor to still underscore
#define LCD_CURSOR_BAR       0b00001110
// Set cursor to blinking block
#define LCD_CURSOR_BLINK     0b00001101
// Set cursor to both of the above at once
#define LCD_CURSOR_BOTH      (LCD_CURSOR_BAR | LCD_CURSOR_BLINK)

// Move cursor
#define LCD_MOVE_LEFT  0b00010000
#define LCD_MOVE_RIGHT 0b00010100

// Shift display
#define LCD_SHIFT_LEFT  0b00011000
#define LCD_SHIFT_RIGHT 0b00011100

// Set iface to 5x7 font, 1-line
#define LCD_IFACE_4BIT_1LINE 0b00100000
#define LCD_IFACE_8BIT_1LINE 0b00110000
// Set iface to 5x7 font, 2-line
#define LCD_IFACE_4BIT_2LINE 0b00101000
#define LCD_IFACE_8BIT_2LINE 0b00111000


// Start address of rows
const uint8_t LCD_ROW_ADDR[] = {0x00, 0x40, 0x14, 0x54};

// prototypes

// --- PUBLIC API ---

/** Init the display */
void lcd_init();
/** Write a command */
void lcd_write_command(const uint8_t bb);
/** Write data byte */
void lcd_write_data(const uint8_t bb);
/** Read busy flag & address */
uint8_t lcd_read_bf_addr();
/** Read byte from ram */
uint8_t lcd_read_ram();
/** Show string */
void lcd_str(char* str);
/** Show string at X, Y */
#define lcd_str_xy(x, y, str_p) do { lcd_xy((x), (y)); lcd_str((str_p)); } while(0)
/** Show char */
void lcd_char(const char c);
/** Show char at X, Y */
#define lcd_char_xy(x, y, c) do { lcd_xy((x), (y)); lcd_char((c)); } while(0)
/** Move cursor to X, Y */
void lcd_xy(const uint8_t x, const uint8_t y);
/** Set address in CGRAM */
void lcd_set_addr_cgram(const uint8_t acg);
/** Set address in DDRAM */
void lcd_set_addr(const uint8_t add);

/** Set cursor */
#define CURSOR_NONE  0b00
#define CURSOR_BAR   0b10
#define CURSOR_BLINK 0b01
#define CURSOR_BOTH  0b11
void lcd_cursor(uint8_t type);

/** Disable / enable, preserving cursor */
void lcd_disable();
void lcd_enable();

/** Define a custom glyph */
void lcd_define_glyph(const uint8_t index, const uint8_t* array);


// Internals
void _lcd_mode_r();
void _lcd_mode_w();
void _lcd_clk();
void _lcd_wait_bf();
void _lcd_write_byte(uint8_t bb);
uint8_t _lcd_read_byte();


// Write utilities
#define _lcd_write_low(bb)  _lcd_write_nibble((bb) & 0x0F)
#define _lcd_write_high(bb) _lcd_write_nibble(((bb) & 0xF0) >> 4)
#define _lcd_write_nibble(nib) do {                \
	write_pin(LCD_PIN_D7, get_bit((nib), 3));     \
	write_pin(LCD_PIN_D6, get_bit((nib), 2));     \
	write_pin(LCD_PIN_D5, get_bit((nib), 1));     \
	write_pin(LCD_PIN_D4, get_bit((nib), 0));     \
} while(0)



// 0 W, 1 R
bool _lcd_mode;

/** Initialize the display */
void lcd_init()
{
	// configure pins as output
	as_output(LCD_PIN_E);
	as_output(LCD_PIN_RW);
	as_output(LCD_PIN_RS);
	_lcd_mode = 1;  // force data pins to output
	_lcd_mode_w();

	// Magic sequence to enter 4-bit mode
	_delay_ms(16);
	_lcd_write_nibble(0b0011);
	_lcd_clk();
	_delay_ms(5);
	_lcd_clk();
	_delay_ms(5);
	_lcd_clk();
	_delay_ms(5);
	_lcd_write_nibble(0b0010);
	_lcd_clk();
	_delay_us(100);

	// Configure the display
	lcd_write_command(LCD_IFACE_4BIT_2LINE);
	lcd_write_command(LCD_DISABLE);
	lcd_write_command(LCD_CLEAR);
	lcd_write_command(LCD_MODE_INC);

	lcd_enable();
}



/** Send a pulse on the ENABLE line */
void _lcd_clk()
{
	pin_up(LCD_PIN_E);
	delay_ns(420);
	pin_down(LCD_PIN_E);
}


/** Enter READ mode */
void _lcd_mode_r()
{
	if (_lcd_mode == 1) return;  // already in R mode

	pin_up(LCD_PIN_RW);

	as_input_pu(LCD_PIN_D7);
	as_input_pu(LCD_PIN_D6);
	as_input_pu(LCD_PIN_D5);
	as_input_pu(LCD_PIN_D4);

	_lcd_mode = 1;
}


/** Enter WRITE mode */
void _lcd_mode_w()
{
	if (_lcd_mode == 0) return;  // already in W mode

	pin_down(LCD_PIN_RW);

	as_output(LCD_PIN_D7);
	as_output(LCD_PIN_D6);
	as_output(LCD_PIN_D5);
	as_output(LCD_PIN_D4);

	_lcd_mode = 0;
}


/** Read a byte */
uint8_t _lcd_read_byte()
{
	_lcd_mode_r();

	uint8_t res = 0;

	_lcd_clk();
	res = (read_pin(LCD_PIN_D7) << 7) | (read_pin(LCD_PIN_D6) << 6) | (read_pin(LCD_PIN_D5) << 5) | (read_pin(LCD_PIN_D4) << 4);

	_lcd_clk();
	res |= (read_pin(LCD_PIN_D7) << 3) | (read_pin(LCD_PIN_D6) << 2) | (read_pin(LCD_PIN_D5) << 1) | (read_pin(LCD_PIN_D4) << 0);

	return res;
}


/** Write an instruction byte */
void lcd_write_command(uint8_t bb)
{
	_lcd_wait_bf();
	pin_down(LCD_PIN_RS);  // select instruction register
	_lcd_write_byte(bb);    // send instruction byte
}


/** Write a data byte */
void lcd_write_data(uint8_t bb)
{
	_lcd_wait_bf();
	pin_up(LCD_PIN_RS);  // select data register
	_lcd_write_byte(bb);  // send data byte
}


/** Read BF & Address */
uint8_t lcd_read_bf_addr()
{
	pin_down(LCD_PIN_RS);
	return _lcd_read_byte();
}


/** Read CGRAM or DDRAM */
uint8_t lcd_read_ram()
{
	pin_up(LCD_PIN_RS);
	return _lcd_read_byte();
}


/** Write a byte using the 8-bit interface */
void _lcd_write_byte(uint8_t bb)
{
	_lcd_mode_w();  // enter W mode

	_lcd_write_high(bb);
	_lcd_clk();

	_lcd_write_low(bb);
	_lcd_clk();
}



/** Wait until the device is ready */
void _lcd_wait_bf()
{
	while(lcd_read_bf_addr() & _BV(7));
}


/** Send a string to LCD */
void lcd_str(char* str_p)
{
	while (*str_p)
		lcd_char(*str_p++);
}


/** Sedn a char to LCD */
void lcd_char(const char c)
{
	lcd_write_data(c);
}


/** Set cursor position */
void lcd_xy(const uint8_t x, const uint8_t y)
{
	lcd_set_addr(LCD_ROW_ADDR[y] + (x));
}


uint8_t _lcd_old_cursor = CURSOR_NONE;
bool _lcd_enabled = false;

/** Set LCD cursor. If not enabled, only remember it. */
void lcd_cursor(uint8_t type)
{
	_lcd_old_cursor = (type & CURSOR_BOTH);

	if (_lcd_enabled) lcd_write_command(LCD_CURSOR_NONE | _lcd_old_cursor);
}


/** Display display (preserving cursor) */
void lcd_disable()
{
	lcd_write_command(LCD_DISABLE);
	_lcd_enabled = false;
}


/** Enable display (restoring cursor) */
void lcd_enable()
{
	_lcd_enabled = true;
	lcd_cursor(_lcd_old_cursor);
}


/** Define a glyph */
void lcd_define_glyph(const uint8_t index, const uint8_t* array)
{
	lcd_set_addr_cgram(index * 8);
	for (uint8_t i = 0; i < 8; ++i)	{
		lcd_write_data(array[i]);
	}
}


/** Define a glyph */
void lcd_define_glyph_pgm(const uint8_t index, const uint8_t* array)
{
	lcd_set_addr_cgram(index * 8);
	for (uint8_t i = 0; i < 8; ++i)	{
		lcd_write_data(pgm_read_byte(&array[i]));
	}
}


/** Set address in CGRAM */
void lcd_set_addr_cgram(const uint8_t acg)
{
	lcd_write_command(0b01000000 | ((acg) & 0b00111111));
}


/** Set address in DDRAM */
void lcd_set_addr(const uint8_t add)
{
	lcd_write_command(0b10000000 | ((add) & 0b01111111));
}
