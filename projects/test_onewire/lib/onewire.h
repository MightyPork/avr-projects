#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#include "lib/iopins.h"
#include "lib/uart.h"
#include "lib/stream.h"

#define SKIP_ROM  0xCC
#define CONVERT_T 0x44
#define READ_SCRATCHPAD 0xBE


/** Perform bus reset. Returns true if any device is connected */
bool ow_reset(const uint8_t pin)
{
	as_output_n(pin);
	pin_low_n(pin);
	_delay_us(480);

	as_input_pu_n(pin);
	_delay_us(70);

	const bool a = get_pin_n(pin);

	_delay_us(410);

	return a;
}


/** Send a single bit */
void ow_tx_bit(const uint8_t pin, const bool bit)
{
	as_output_n(pin);
	pin_low_n(pin);

	if (bit) {
		_delay_us(6);
		as_input_pu_n(pin);
		_delay_us(64);
	} else {
		_delay_us(60);
		as_input_pu_n(pin);
		_delay_us(10);
	}
}


/** Send a single byte */
void ow_send(const uint8_t pin, const uint8_t byte)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		ow_tx_bit(pin, (byte >> i) & 0x01);
	}
}


/** Read a single bit */
bool ow_rx_bit(const uint8_t pin)
{
	as_output_n(pin);
	pin_low_n(pin);
	_delay_us(6);

	as_input_pu_n(pin);
	_delay_us(9);

	const bool a = get_pin_n(pin);

	_delay_us(55);

	return a;
}


/** Read a single byte */
uint8_t ow_read(const uint8_t pin)
{
	uint8_t byte = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		byte = (byte >> 1) | (ow_rx_bit(pin) << 7);
	}

	return byte;
}


/** Wait until the device is ready. Returns false on timeout */
bool ow_wait_ready(const uint8_t pin)
{
	uint16_t timeout = 700;
	as_input_pu_n(pin);

	while (--timeout > 0)
	{
		if (is_high_n(pin)) return true;
		_delay_ms(1);
	}

	return false;
}


/** Read bytes into an array */
void ow_read_arr(const uint8_t pin, uint8_t* array, const uint8_t count)
{
	for (uint8_t i = 0; i < count; i++)
	{
		array[i] = ow_read(pin);
	}
}



// ---------- CRC utils ----------

/*
 Dallas 1-wire CRC routines for Arduino with examples of usage.
 The 16-bit routine is new.
 The 8-bit routine is from http://github.com/paeaetech/paeae/tree/master/Libraries/ds2482/

 Copyright (C) 2010 Kairama Inc

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
// Dallas 1-wire 16-bit CRC calculation. Developed from Maxim Application Note 27.

/** Compute a CRC16 checksum */
uint16_t crc16( uint8_t *data, uint8_t len)
{
	uint16_t crc = 0;

	for (uint8_t i = 0; i < len; i++)
	{
		uint8_t inbyte = data[i];
		for (uint8_t j = 0; j < 8; j++)
		{
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc = crc >> 1;
			if (mix)
				crc = crc ^ 0xA001;

			inbyte = inbyte >> 1;
		}
	}
	return crc;
}


// The 1-Wire CRC scheme is described in Maxim Application Note 27:
// "Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products"

/** Compute a CRC8 checksum */
uint8_t crc8(uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;

	for (uint8_t i = 0; i < len; i++)
	{
		uint8_t inbyte = addr[i];
		for (uint8_t j = 0; j < 8; j++)
		{
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix)
					crc ^= 0x8C;

			inbyte >>= 1;
		}
	}

	return crc;
}


// --- utils for DS1820 ---

#define TEMP_ERROR -32768

/** Read temperature in 0.0625°C, or TEMP_ERROR on error */
int16_t ds1820_read_temp(uint8_t pin)
{
	ow_send(pin, READ_SCRATCHPAD);
	uint8_t bytes[9];
	ow_read_arr(pin, bytes, 9);

	put_nl(uart);

	uint8_t crc = crc8(bytes, 8);
	if (crc != bytes[8]) {
		return TEMP_ERROR;
	} else {
		int16_t a = ((bytes[1] << 8) | bytes[0]) >> 1;
		a = a << 4;
		a += (16 - bytes[6]) & 0x0F;
		a -= 0x04;

		return a;
	}
}

/** Read temperature in 0.1°C, or TEMP_ERROR on error */
int16_t ds1820_read_temp_c(uint8_t pin)
{
	int32_t temp = ds1820_read_temp(pin);

	if (temp == TEMP_ERROR)
		return TEMP_ERROR;

	temp *= 625;
	uint16_t rem = temp % 1000;
	temp /= 1000;
	if (rem >= 500) temp++;

	return (int16_t) temp;
}

