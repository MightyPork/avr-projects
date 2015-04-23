#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>

#include "uart.h"

void _uart_init_do(uint16_t ubrr) {
	/*Set baud rate */
	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) ubrr;

	// Enable Rx and Tx
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// 8-bit data, 1 stop bit
	UCSR0C = (0b11 << UCSZ00);
}


/** Enable or disable RX ISR */
void uart_isr_rx(bool yes)
{
	if(yes) {
		UCSR0B |= (1 << RXCIE0);
	} else {
		UCSR0B &= ~(1 << RXCIE0);
	}
}

/** Enable or disable TX ISR (1 byte is sent) */
void uart_isr_tx(bool yes)
{
	if(yes) {
		UCSR0B |= (1 << TXCIE0);
	} else {
		UCSR0B &= ~(1 << TXCIE0);
	}
}

/** Enable or disable DRE ISR (all is sent) */
void uart_isr_dre(bool yes)
{
	if(yes) {
		UCSR0B |= (1 << UDRIE0);
	} else {
		UCSR0B &= ~(1 << UDRIE0);
	}
}




/** Send byte over UART */
void uart_tx(uint8_t data)
{
	// Wait for transmit buffer
	while (!uart_tx_ready());
	// send it
	UDR0 = data;
}

/** Send string over UART */
void uart_puts(const char* str)
{
	while (*str) {
		uart_tx(*str++);
	}
}

/** Send progmem string over UART */
void uart_puts_pgm(const char* str)
{
	char c;
	while ((c = pgm_read_byte(str++))) {
		uart_tx(c);
	}
}

/** Receive one byte over UART */
uint8_t uart_rx()
{
	// Wait for data to be received
	while (!uart_rx_ready());
	// Get and return received data from buffer
	return UDR0;
}

/** Clear receive buffer */
void uart_flush()
{
	uint8_t dummy;
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}

