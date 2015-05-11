#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#include "iopins.h"
#include "dht11.h"

/** Read one bit */
bool _dht11_rxbit(const uint8_t pin)
{
	// Wait until start of pulse
	while (is_low_n(pin));

	uint8_t cnt = 0;
	while (is_high_n(pin))
	{
		cnt++;
		_delay_us(5);
	}

	return (cnt > 8);
}


/** Read one byte */
uint8_t _dht11_rxbyte(const uint8_t pin)
{
	uint8_t byte = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		if (_dht11_rxbit(pin))
			byte |= (1 << (7 - i));
	}

	return byte;
}


/** Read tehmperature and humidity from the DHT11, returns false on failure */
bool dht11_read(const uint8_t pin, dht11_result_t* result)
{
	// bus down for > 18 ms
	as_output_n(pin);
	pin_low_n(pin);
	_delay_ms(20);

	// bus up for 20-40us
	pin_high_n(pin);
	_delay_us(20);

	// release
	as_input_pu_n(pin);

	// DHT should send 80us LOW & 80us HIGH

	_delay_us(40);
	if (!is_low_n(pin))
		return false; // init error

	_delay_us(80);
	if (!is_high_n(pin))
		return false; // init error

	// skip to start of first bit
	_delay_us(50);

	// Receive 5 data bytes (Rh int, Rh dec, Temp int, Temp dec, Checksum)
	// Decimal bytes are zero for DHT11 -> we can ignore them.
	uint8_t bytes[5];
	uint8_t sum = 0;

	for (uint8_t i = 0; i < 5; i++)
	{
		uint8_t b = _dht11_rxbyte(pin);
		bytes[i] = b;
		if (i < 4) sum += b;
	}

	// Verify checksum
	if (sum != bytes[4]) return false;

	result->rh = bytes[0];
	result->temp = bytes[2];

	return true;
}
