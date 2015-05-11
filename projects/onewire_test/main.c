#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#include "lib/iopins.h"
#include "lib/uart.h"
#include "lib/stream.h"
#include "lib/onewire.h"

#define T 2

void main()
{
	uart_init(9600);
	uart_puts_P(PSTR("*** OneWire test ***\r\n"));

	while(1)
	{
		_delay_ms(100);

		ow_reset(T);
		ow_send(T, SKIP_ROM);
		ow_send(T, CONVERT_T);

		if(!ow_wait_ready(T)) {
			uart_puts_P(PSTR("Wait timeout!\r\n"));
			continue;
		}

		ow_reset(T);
		ow_send(T, SKIP_ROM);

		int16_t temp = ds1820_read_temp_c(T);

		if (temp == TEMP_ERROR) {
			uart_puts_P(PSTR("Read error!\r\n"));
		} else {
			put_i16f(uart, temp, 1);
			uart_puts_P(PSTR(" °C\r\n"));
		}
	}
}
