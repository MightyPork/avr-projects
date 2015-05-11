#pragma once

//
// Utilities for UART communication.
//
// First, init uart with desired baud rate using uart_init(baud).
// Then enable interrupts you want with uart_isr_XXX().
//

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>

#include "stream.h"

// Shared UART stream object
// Can be used with functions from stream.h once UART is initialized
extern STREAM* uart;

/** Init UART for given baudrate */
void _uart_init_do(uint16_t ubrr); // internal, needed for the macro.
#define uart_init(baud) _uart_init_do(F_CPU / 16 / (baud) - 1)

/** Check if there's a byte in the RX register */
#define uart_rx_ready() (0 != (UCSR0A & (1 << RXC0)))

/** Check if transmission of everything is done */
#define uart_tx_ready() (0 != (UCSR0A & (1 << UDRE0)))



// Enable UART interrupts

/** Enable or disable RX ISR */
void uart_isr_rx(bool enable);

/** Enable or disable TX ISR (1 byte is sent) */
void uart_isr_tx(bool enable);

/** Enable or disable DRE ISR (all is sent) */
void uart_isr_dre(bool enable);



// Basic IO

/** Receive one byte over UART */
uint8_t uart_rx();

/** Send byte over UART */
#define uart_putc(data) uart_tx((data))
void uart_tx(uint8_t data);

/** Clear receive buffer */
void uart_flush();



// Strings

/** Send string over UART */
void uart_puts(const char* str);

/** Send progmem string over UART */
void uart_puts_P(const char* str);



//
// ANSI / VT100 utilities for UART
//
// To use this, first call uart_init(baud) and vt_init()
// To print stuff on the screen, use uart_puts() etc from uart.h
//


// INIT

/** Initialize helper variables */
void vt_init();

/** Reset state and clear screen */
void vt_reset();



// CURSOR MOVE

/** Move cursor to top left corner */
void vt_home();

/** Jump to a location on the screen */
void vt_goto(uint8_t x, uint8_t y);

/** Jump to given X, keep Y */
void vt_goto_x(uint8_t x);

/** Jump to given Y, keep X */
void vt_goto_y(uint8_t y);

/** Move cursor relative to current location */
void vt_move(int8_t x, int8_t y);

/** Move cursor horizontally */
void vt_move_x(int8_t x);

/** Move cursor vertically */
void vt_move_y(int8_t y);

/** Move cursor up y cells */
void vt_up(uint8_t y);

/** Move cursor down y cells */
void vt_down(uint8_t y);

/** Move cursor left x cells */
void vt_left(uint8_t x);

/** Move cursor right x cells */
void vt_right(uint8_t x);



// SCROLLING

/** Scroll y lines down (like up/down, but moves window if needed) */
void vt_scroll(int8_t down);

/** Set scrolling region (lines) */
void vt_scroll_set(uint8_t from, uint8_t to);


/** Sets scrolling region to the entire screen. */
void vt_scroll_reset();


// COLOR

#define VT_BLACK 0
#define VT_RED 1
#define VT_GREEN 2
#define VT_YELLOW 3
#define VT_BLUE 4
#define VT_MAGENTA 5
#define VT_CYAN 6
#define VT_WHITE 7

/** Set color 0..7 */
void vt_color(uint8_t fg, uint8_t bg);

/** Set FG color 0..7 */
void vt_color_fg(uint8_t fg);

/** Set BG color 0..7 */
void vt_color_bg(uint8_t bg);

/** Set color to white on black */
void vt_color_reset();



// STYLES

#define VT_BOLD 1
#define VT_UNDERLINE 2
#define VT_BLINK 4
#define VT_REVERSE 8
#define VT_ITALIC 16
#define VT_FAINT 32

/** Enable or disable a text attribute */
void vt_attr(uint8_t attribute, bool on);

/** Disable all text attributes (excluding color) */
void vt_attr_reset();



// SAVE & RESTORE

/** Save cursor position & text attributes */
void vt_save();

/** Restore cursor to saved values */
void vt_restore();



// MODIFY


/** Insert blank lines febore the current line */
void vt_insert_lines(uint8_t count);

/** Delete lines from the current line down */
void vt_delete_lines(uint8_t count);

/** Insert empty characters at cursor */
void vt_insert_chars(uint8_t count);

/** Delete characters at cursor */
void vt_delete_chars(uint8_t count);



// ERASING

/** Clear the screen */
void vt_clear();

/** Erase to the end of line */
void vt_erase_forth();

/** Erase line to cursor */
void vt_erase_back();

/** Erase entire line */
void vt_erase_line();

/** Erase screen below the line */
void vt_erase_above();

/** Erase screen above the line */
void vt_erase_below();



// KEY HANDLER

// Special keys from key handler
#define VK_LEFT   68
#define VK_RIGHT  67
#define VK_UP     65
#define VK_DOWN   66
#define VK_DELETE 51
#define VK_INSERT 50
#define VK_PGUP   53
#define VK_PGDN   54
#define VK_HOME   72
#define VK_END    70
#define VK_F1 80
#define VK_F2 81
#define VK_F3 82
#define VK_F4 83
#define VK_BACKSPACE  8
#define VK_TAB        9
#define VK_ENTER     13
#define VK_ESC       27

void vt_handle_key(uint8_t c);
void vt_set_key_handler(void (*handler)(uint8_t, bool));
