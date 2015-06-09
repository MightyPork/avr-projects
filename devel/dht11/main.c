#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#include "lib/iopins.h"
#include "lib/uart.h"
#include "lib/stream.h"
#include "lib/dht11.h"

void main()
{
	uart_init(9600);
	uart_puts_P(PSTR("*** DHT11 test ***\r\n"));

	while(1)
	{
		dht11_result_t result;

		if (dht11_read(2, &result)) {
			// Temperature
			put_i8(uart, result.temp);
			uart_puts_P(PSTR(" °C, "));
			// Humidity
			put_i8(uart, result.rh);
			uart_puts_P(PSTR(" %r.H\r\n"));
		} else {
			uart_puts_P(PSTR("Read error!\r\n"));
		}

		_delay_ms(500);
	}
}
