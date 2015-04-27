#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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


/** Receive one byte over UART */
uint8_t uart_rx()
{
	// Wait for data to be received
	while (!uart_rx_ready());
	// Get and return received data from buffer
	return UDR0;
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


/** Clear receive buffer */
void uart_flush()
{
	uint8_t dummy;
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}


/** Send CRLF */
void uart_nl()
{
	uart_tx(13);
	uart_tx(10);
}


char tmpstr[12]; // buffer for number rendering

void _uart_putnf(const uint8_t places);


/** Send signed int8 */
void uart_putu(const uint8_t num)
{
	utoa(num, tmpstr, 10);
	uart_puts(tmpstr);
}


/** Send unsigned int8 */
void uart_putn(const int8_t num)
{
	itoa(num, tmpstr, 10);
	uart_puts(tmpstr);
}


/** Send unsigned int as float */
void uart_putiu(const uint16_t num, const uint8_t places)
{
	if (!places) {
		utoa(num, tmpstr, 10);
		uart_puts(tmpstr);
	} else {
		utoa(num, tmpstr, 10);
		_uart_putnf(places);
	}
}


/** Send signed int as float */
void uart_puti(const int16_t num, const uint8_t places)
{
	if (!places) {
		itoa(num, tmpstr, 10);
		uart_puts(tmpstr);
	} else {
		if (num < 0) {
			uart_tx('-');
			itoa(-num, tmpstr, 10);
		} else {
			itoa(num, tmpstr, 10);
		}

		_uart_putnf(places);
	}
}


/** Send unsigned long as float */
void uart_putlu(const uint32_t num, const uint8_t places)
{
	if (!places) {
		ultoa(num, tmpstr, 10);
		uart_puts(tmpstr);
	} else {
		ultoa(num, tmpstr, 10);
		_uart_putnf(places);
	}
}


/** Send signed long as float */
void uart_putl(const int32_t num, const uint8_t places)
{
	if (!places) {
		ltoa(num, tmpstr, 10);
		uart_puts(tmpstr);
	} else {
		if (num < 0) {
			uart_tx('-');
			ltoa(-num, tmpstr, 10);
		} else {
			ltoa(num, tmpstr, 10);
		}

		_uart_putnf(places);
	}
}


/** Print number in tmp string as float with given decimal point position */
void _uart_putnf(const uint8_t places)
{
	// measure text length
	uint8_t len = 0;
	while(tmpstr[len] != 0) len++;

	int8_t at = len - places;

	// print virtual zeros
	if (at <= 0) {
		uart_tx('0');
		uart_tx('.');
		while(at <= -1) {
			uart_tx('0');
			at++;
		}
		at = -1;
	}

	// print the number
	uint8_t i = 0;
	while(i < len) {
		if (at-- == 0) {
			uart_tx('.');
		}

		uart_tx(tmpstr[i++]);
	}
}

