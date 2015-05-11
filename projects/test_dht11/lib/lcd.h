#pragma once

//  HD44780 LCD display driver - 4-bit mode
//
//  LCD pins are configured using a file lcd_config.h, which you
//  have to add next to your main.c file.
//
//  Content can be something like this:
//
//

#include <stdint.h>
#include <stdbool.h>

#include "stream.h"

// Your file with configs
#include "lcd_config.h"
/*
	#define LCD_RS 10
	#define LCD_RW 11
	#define LCD_E  12
	#define LCD_D4 13
	#define LCD_D5 14
	#define LCD_D6 15
	#define LCD_D7 16
*/



// Shared LCD stream object
// Can be used with functions from stream.h once LCD is initialized
extern STREAM* lcd;


// --- Commands ---

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


/** Initialize the display */
void lcd_init();

/** Write an instruction byte */
void lcd_command(uint8_t bb);

/** Write a data byte */
void lcd_write(uint8_t bb);

/** Read BF & Address */
uint8_t lcd_read_bf_addr();

/** Read CGRAM or DDRAM */
uint8_t lcd_read();

/** Send a string to LCD */
void lcd_puts(char* str_p);

/** Send a string to LCD from program memory */
void lcd_puts_P(const char* str_p);

/** Sedn a char to LCD */
void lcd_putc(const char c);

/** Show string at X, Y */
#define lcd_puts_xy(x, y, str_p) do { lcd_xy((x), (y)); lcd_puts((str_p)); } while(0)

/** Show string at X, Y */
#define lcd_puts_xy_P(x, y, str_p) do { lcd_xy((x), (y)); lcd_puts_P((str_p)); } while(0)

/** Show char at X, Y */
#define lcd_putc_xy(x, y, c) do { lcd_xy((x), (y)); lcd_putc((c)); } while(0)

/** Set cursor position */
void lcd_xy(const uint8_t x, const uint8_t y);

/** Set LCD cursor. If not enabled, only remember it. */
#define CURSOR_NONE  0b00
#define CURSOR_BAR   0b10
#define CURSOR_BLINK 0b01
#define CURSOR_BOTH  0b11
void lcd_cursor(uint8_t type);

/** Display display (preserving cursor) */
void lcd_disable();

/** Enable display (restoring cursor) */
void lcd_enable();

/** Go home */
void lcd_home();

/** Clear the screen */
void lcd_clear();

/** Define a glyph - 8 bytes, right 5 bits are used */
void lcd_glyph(const uint8_t index, const uint8_t* array);

/** Define a glyph that's in PROGMEM */
void lcd_glyph_P(const uint8_t index, const uint8_t* array);

/** Set address in CGRAM */
void lcd_addr_cg(const uint8_t acg);

/** Set address in DDRAM */
void lcd_addr(const uint8_t add);
