#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "lib/uart.h"
#include "lib/uart_ansi.h"

ISR(USART_RX_vect)
{
	vt_handle_key(uart_rx());
}


void key_handler(uint8_t code, bool special)
{
	if (special) {
		uart_puts_pgm(PSTR("Special key: "));
	} else {
		uart_puts_pgm(PSTR("Character: "));
	}

	uart_putc(code);
	uart_putc(32);
	uart_putu(code);
	uart_nl();
}


void main()
{
	uart_init(9600);
	uart_isr_rx(1);
	vt_init();

	vt_set_key_handler(&key_handler);

	sei();

	uart_puts_pgm(PSTR("UART key handler test!\r\n"));

	while(1);
}
