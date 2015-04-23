#pragma once

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>

/** Init UART for given baudrate */
#define uart_init(baud) _uart_init_do(F_CPU / 16 / (baud) - 1)

void _uart_init_do(uint16_t ubrr);

/** Check if there's a byte in the RX register */
#define uart_rx_ready() (0 != (UCSR0A & (1 << RXC0)))

/** Check if transmission of everything is done */
#define uart_tx_ready() (0 != (UCSR0A & (1 << UDRE0)))

/** Enable or disable RX ISR */
void uart_isr_rx(bool enable);

/** Enable or disable TX ISR (1 byte is sent) */
void uart_isr_tx(bool enable);

/** Enable or disable DRE ISR (all is sent) */
void uart_isr_dre(bool enable);

/** Send byte over UART */
#define uart_putc(data) uart_tx(data)
void uart_tx(uint8_t data);

/** Send string over UART */
void uart_puts(const char* str);

/** Send progmem string over UART */
void uart_puts_pgm(const char* str);

/** Receive one byte over UART */
uint8_t uart_rx();

/** Clear receive buffer */
void uart_flush();
