#include <avr/pgmspace.h>
#include <util/delay.h>

#include "lib/uart.h"
#include "lib/stream.h"
#include "lib/sd.h"
#include "lib/sd_fat.h"
#include "lib/sd_blockdev.h"

void test_lowlevel()
{
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
		put_str_P(uart, PSTR("Failed to read.\r\n"));
		while (1);
	}
}


void try_reading_dirs(FAT16_FILE* f )
{
	char buf[200];

	do
	{
		if (!fat16_is_regular(f)) continue;

		if (f->type == FT_SUBDIR)
		{
			vt_color_fg(VT_MAGENTA);
			uart_puts_P(PSTR("\r\n--- DIRECTORY: "));
			uart_puts(fat16_dispname(f, buf));
			uart_puts_P(PSTR(" ---\r\n"));
			vt_color_reset();

			FSAVEPOS p = fat16_savepos(f);

			if (fat16_opendir(f))
			{
				do
				{
					if (!fat16_is_regular(f)) continue;

					vt_color_fg(VT_GREEN);
					uart_puts_P(PSTR(">> --- File: "));
					uart_puts(fat16_dispname(f, buf));
					uart_puts_P(PSTR(" ---\r\n"));
					vt_color_reset();
				}
				while (fat16_next(f));
			}

			fat16_reopen(f, &p);
		}
		else
		{
			vt_color_fg(VT_YELLOW);
			uart_puts_P(PSTR("--- File: "));
			uart_puts(fat16_dispname(f, buf));
			uart_puts_P(PSTR(" ---\r\n"));
			vt_color_reset();

			uint16_t len = 0;
			if ((len = fat16_read(f, buf, 199)))
			{
				buf[len] = 0;
				uart_puts(buf);
				put_nl(uart);
			}
			else
			{
				uart_puts_P(PSTR("COULD NOT READ.\n"));
			}
		}
	}
	while (fat16_next(f));
}

// Init a file pointer */
FAT16_FILE _f;
FAT16_FILE* f = &_f;

void main()
{
	uart_init(9600);
	vt_init();

	vt_color_fg(VT_CYAN);
	vt_attr(VT_BOLD, 1);
	uart_puts_P(PSTR("\r\n*** FAT16 on SD card test ***\r\n"));
	vt_color_reset();
	vt_attr_reset();

	if (!(sdfat_init()))
	{
		uart_puts_P(PSTR("Failed to init.\r\n"));
		while (1);
	}

	uart_puts_P(PSTR("SD card with FAT16 inited.\r\n"));



	// Read and print disk label
	char lbl[16];
	sdfat_disk_label(lbl);
	put_str(uart, lbl);
	put_nl(uart);

	sdfat_root(f);

	try_reading_dirs(f);

//	if (fat16_find(f, "TESTFIL2.TXT"))
//	{
//		fat16_seek(f, f->size);
//		fat16_write(f, "ONE MORE THING.\n", 16);
//		sdfat_flush();

//		uart_puts_P(PSTR("Write OK!\r\n"));
//	}
//	else
//	{
//		uart_puts_P(PSTR("Failed to open file.\r\n"));
//		while (1);
//	}


	while (1);
}
