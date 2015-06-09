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
	uint8_t x = uart_rx();
	uart_tx(x);
}

void main()
{
	uart_init(9600);
	uart_isr_rx(1);
	sei();
	while(1) {
		//uart_puts_P(PSTR("TEST"));
	}
}
