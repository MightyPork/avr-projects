#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "calc.h"
#include "pins.h"
#include "nsdelay.h"
#include "lcd.h"
#include "lcd_config.h"

// Start address of rows
const uint8_t LCD_ROW_ADDR[] = {0x00, 0x40, 0x14, 0x54};

// Internal prototypes
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
	write_pin(LCD_D7, get_bit((nib), 3));     \
	write_pin(LCD_D6, get_bit((nib), 2));     \
	write_pin(LCD_D5, get_bit((nib), 1));     \
	write_pin(LCD_D4, get_bit((nib), 0));     \
} while(0)


// 0 W, 1 R
bool _lcd_mode;


/** Initialize the display */
void lcd_init()
{
	// configure pins as output
	as_output(LCD_E);
	as_output(LCD_RW);
	as_output(LCD_RS);
	_lcd_mode = 1;  // force data pins to output
	_lcd_mode_w();

	// Magic sequence to invoke Cthulhu (or enter 4-bit mode)
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

	// mark as enabled
	lcd_enable();
}


/** Send a pulse on the ENABLE line */
void _lcd_clk()
{
	pin_up(LCD_E);
	delay_ns(420);
	pin_down(LCD_E);
}


/** Enter READ mode */
void _lcd_mode_r()
{
	if (_lcd_mode == 1) return;  // already in R mode

	pin_up(LCD_RW);

	as_input_pu(LCD_D7);
	as_input_pu(LCD_D6);
	as_input_pu(LCD_D5);
	as_input_pu(LCD_D4);

	_lcd_mode = 1;
}


/** Enter WRITE mode */
void _lcd_mode_w()
{
	if (_lcd_mode == 0) return;  // already in W mode

	pin_down(LCD_RW);

	as_output(LCD_D7);
	as_output(LCD_D6);
	as_output(LCD_D5);
	as_output(LCD_D4);

	_lcd_mode = 0;
}


/** Read a byte */
uint8_t _lcd_read_byte()
{
	_lcd_mode_r();

	uint8_t res = 0;

	_lcd_clk();
	res = (read_pin(LCD_D7) << 7) | (read_pin(LCD_D6) << 6) | (read_pin(LCD_D5) << 5) | (read_pin(LCD_D4) << 4);

	_lcd_clk();
	res |= (read_pin(LCD_D7) << 3) | (read_pin(LCD_D6) << 2) | (read_pin(LCD_D5) << 1) | (read_pin(LCD_D4) << 0);

	return res;
}


/** Write an instruction byte */
void lcd_write_command(uint8_t bb)
{
	_lcd_wait_bf();
	pin_down(LCD_RS);  // select instruction register
	_lcd_write_byte(bb);    // send instruction byte
}


/** Write a data byte */
void lcd_write_data(uint8_t bb)
{
	_lcd_wait_bf();
	pin_up(LCD_RS);  // select data register
	_lcd_write_byte(bb);  // send data byte
}


/** Read BF & Address */
uint8_t lcd_read_bf_addr()
{
	pin_down(LCD_RS);
	return _lcd_read_byte();
}


/** Read CGRAM or DDRAM */
uint8_t lcd_read_ram()
{
	pin_up(LCD_RS);
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
	uint8_t d = 0;
	while(d++ < 120 && lcd_read_bf_addr() & _BV(7))
		_delay_us(1);
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


/** Go home */
void lcd_home()
{
	lcd_write_command(LCD_HOME);
}


/** Clear the screen */
void lcd_clear()
{
	lcd_write_command(LCD_CLEAR);
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
