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
void uart_puts_pgm(const char* str);


// Numbers

/** Send unsigned int */
void uart_putn(const int8_t num);

/** Send signed int */
void uart_putu(const uint8_t num);

/** Send unsigned int */
void uart_puti(const int16_t num, const uint8_t places);

/** Send signed int */
void uart_putiu(const uint16_t num, const uint8_t places);

/** Send unsigned long */
void uart_putlu(const uint32_t num, const uint8_t places);

/** Send signed long */
void uart_putl(const int32_t num, const uint8_t places);

// Extras

/** Send CRLF */
void uart_nl();
