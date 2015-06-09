#include <avr/pgmspace.h>
#include <util/delay.h>

#include "lib/uart.h"
#include "lib/stream.h"
#include "lib/sd.h"

void main()
{
	uart_init(9600);
	uart_puts_P(PSTR("*** SD CARD SPI TEST ***\r\n"));

	if (!sd_init())
	{
		put_str_P(uart, PSTR("Failed to init."));
		while (1);
	}


	uint8_t text[512];



	put_str_P(uart, PSTR("\r\nReading...\r\n"));
	if (sd_read(0, 0, text, 0, 512))
	{
		put_bytes(uart, text, 512);
		put_nl(uart);
		put_nl(uart);

		for (uint8_t i = 0; i < 60; i++)
		{
			text[i] = ('A' + i);
		}

		put_str_P(uart, PSTR("\r\nWriting...\r\n"));
		sd_write(1, text);
		put_str_P(uart, PSTR("\r\nWrite done.\r\n"));

		for (uint16_t i = 0; i < 512; i++)
		{
			text[i] = 0;
		}

		if (sd_read(1, 0, text, 0, 512))
		{
			put_str_P(uart, PSTR("\r\nWritten, result: \r\n"));
			put_bytes(uart, text, 512);
			put_nl(uart);
			put_nl(uart);
		}
		else
		{
			put_str_P(uart, PSTR("\r\nRead failed.\r\n"));
		}
	}
	else
	{
		put_str_P(uart, PSTR("Failed to read."));
		while (1);
	}

	while (1);
}
