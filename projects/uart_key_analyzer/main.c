#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "lib/uart.h"


ISR(USART_RX_vect)
{
	uint8_t c = uart_rx();

	uart_putc(c);
	uart_putc(' ');
	uart_putn(c);
	uart_nl();
}


void main()
{
	uart_init(9600);
	uart_isr_rx(1);
	sei();

	uart_puts_pgm(PSTR("UART key analyzer\r\n"));

	while(1);
}
