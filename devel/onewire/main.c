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

void main()
{
	uart_init(9600);
	uart_puts_P(PSTR("*** OneWire test ***\r\n"));

	while(1)
	{
		_delay_ms(100);

		if (ds1820_single_measure(2))
		{
			int16_t temp = ds1820_read_temp_c(2);

			if (temp == TEMP_ERROR) {
				// Checksum mismatch
				uart_puts_P(PSTR("Corrupt data!\r\n"));
			} else {
				put_i16f(uart, temp, 1);
				uart_puts_P(PSTR(" °C\r\n"));
			}

		} else {
			uart_puts_P(PSTR("Device not connected!\r\n"));
		}
	}
}
