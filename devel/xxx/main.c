#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "fat16.h"

BLOCKDEV test;

void test_seek(const uint32_t pos)
{
	asm volatile ("");
}

void test_rseek(const int16_t pos)
{
	asm volatile ("");
}

void test_load(void* dest, const uint16_t len)
{
	asm volatile ("");
}

void test_store(const void* source, const uint16_t len)
{
	asm volatile ("");
}

void test_write(const uint8_t b)
{
	asm volatile ("");
}

uint8_t test_read()
{
	return 0;
}

void test_open()
{
	test.read = &test_read;
	test.write = &test_write;

	test.load = &test_load;
	test.store = &test_store;

	test.seek = &test_seek;
	test.rseek = &test_rseek;
}


void main()
{
	test_open();

	// Initialize the FS
	FAT16 fat;
	fat16_init(&test, &fat);

	FAT16_FILE file;
	fat16_root(&fat, &file);

	char str[10];

fat16_disk_label(&fat, str);


// ----------- FILE I/O -------------


/**
 * Move file cursor to a position relative to file start
 * Returns false on I/O error (bad file, out of range...)
 */
fat16_fseek(&file, 1234);


/**
 * Read bytes from file into memory
 * Returns false on I/O error (bad file, out of range...)
 */
fat16_fread(&file, str, 10);


/**
 * Write into file at a "seek" position.
 * "seek" cursor must be within (0..filesize)
 */
fat16_fwrite(&file, str, 10);


/** Go to next file in directory (false = no next file) */
fat16_next(&file);


	while(1) {
		//uart_puts_P(PSTR("TEST"));
	}
}
