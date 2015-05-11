#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "stream.h"
#include "calc.h"


static char tmpstr[20]; // buffer for number rendering


void put_str(const STREAM *p, char* str)
{
	char c;
	while ((c = *str++)) {
		p->tx(c);
	}
}


void put_str_P(const STREAM *p, const char* str)
{
	char c;
	while ((c = pgm_read_byte(str++))) {
		p->tx(c);
	}
}


static void _putnf(const STREAM *p, const uint8_t places);


/** Send signed int8 */
void put_u8(const STREAM *p, const uint8_t num)
{
	utoa(num, tmpstr, 10);
	put_str(p, tmpstr);
}


/** Send unsigned int8 */
void put_i8(const STREAM *p, const int8_t num)
{
	itoa(num, tmpstr, 10);
	put_str(p, tmpstr);
}



/** Send unsigned int */
void put_u16(const STREAM *p, const uint16_t num)
{
	utoa(num, tmpstr, 10);
	put_str(p, tmpstr);
}


/** Send signed int */
void put_i16(const STREAM *p, const int16_t num)
{
	itoa(num, tmpstr, 10);
	put_str(p, tmpstr);
}


/** Send unsigned long */
void put_u32(const STREAM *p, const uint32_t num)
{
	ultoa(num, tmpstr, 10);
	put_str(p, tmpstr);
}


/** Send signed long */
void put_i32(const STREAM *p, const int32_t num)
{
	ltoa(num, tmpstr, 10);
	put_str(p, tmpstr);
}


/** Print number as hex */
void _print_hex(const STREAM *p, uint8_t* start, uint8_t bytes)
{
	for (; bytes > 0; bytes--) {
		uint8_t b = *(start + bytes - 1);

		for(uint8_t j = 0; j < 2; j++) {
			uint8_t x = high_nibble(b);
			b = b << 4;
			if (x < 0xA) {
				p->tx('0' + x);
			} else {
				p->tx('A' + (x - 0xA));
			}
		}
	}
}


/** Send unsigned int8 */
void put_x8(const STREAM *p, const uint8_t num)
{
	_print_hex(p, (uint8_t*) &num, 1);
}


/** Send int as hex */
void put_x16(const STREAM *p, const uint16_t num)
{
	_print_hex(p, (uint8_t*) &num, 2);
}


/** Send long as hex */
void put_x32(const STREAM *p, const uint32_t num)
{
	_print_hex(p, (uint8_t*) &num, 4);
}


/** Send long long as hex */
void put_x64(const STREAM *p, const uint64_t num)
{
	_print_hex(p, (uint8_t*) &num, 8);
}



// float variant doesn't make sense for 8-bit int

/** Send unsigned int as float */
void put_u16f(const STREAM *p, const uint16_t num, const uint8_t places)
{
	utoa(num, tmpstr, 10);
	_putnf(p, places);
}


/** Send signed int as float */
void put_i16f(const STREAM *p, const int16_t num, const uint8_t places)
{
	if (num < 0) {
		p->tx('-');
		itoa(-num, tmpstr, 10);
	} else {
		itoa(num, tmpstr, 10);
	}

	_putnf(p, places);
}


/** Send unsigned long as float */
void put_u32f(const STREAM *p, const uint32_t num, const uint8_t places)
{
	ultoa(num, tmpstr, 10);
	_putnf(p, places);
}


/** Send signed long as float */
void put_i32f(const STREAM *p, const int32_t num, const uint8_t places)
{
	if (num < 0) {
		p->tx('-');
		ltoa(-num, tmpstr, 10);
	} else {
		ltoa(num, tmpstr, 10);
	}

	_putnf(p, places);
}


/** Print number in tmp string as float with given decimal point position */
void _putnf(const STREAM *p, const uint8_t places)
{
	// measure text length
	uint8_t len = 0;
	while(tmpstr[len] != 0) len++;

	int8_t at = len - places;

	// print virtual zeros
	if (at <= 0) {
		p->tx('0');
		p->tx('.');
		while(at <= -1) {
			p->tx('0');
			at++;
		}
		at = -1;
	}

	// print the number
	uint8_t i = 0;
	while(i < len) {
		if (at-- == 0) {
			p->tx('.');
		}

		p->tx(tmpstr[i++]);
	}
}


/** Print CR LF */
void put_nl(const STREAM *p)
{
	p->tx(13);
	p->tx(10);
}
