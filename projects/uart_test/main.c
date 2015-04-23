#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>

#include "lib/meta.h"
#include "lib/uart.h"

const char str[] PROGMEM = "Hello UART!\r\n";


// Echo, but with ROT13.
ISR(USART_RX_vect)
{
	char c = uart_rx();

	if (c == 13) {
		uart_tx(10);
		uart_tx(13);
		return;
	}

	if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M')) {
			c += 13;
	} else
	if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z')) {
			c -= 13;
	}

	uart_tx(c);
}


void SECTION(".init8") init()
{
	uart_init(9600);
	uart_isr_rx(1);
	sei();
}


void main()
{
	uart_puts_pgm(str);

	while(1);
}
