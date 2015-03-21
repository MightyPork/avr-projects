#pragma once

#include <avr/io.h>
#include <stdbool.h>
#include "calc.h"

/** Initialize the ADC */
void adc_init()
{
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);  // 128 prescaler -> 125 kHz
	ADMUX  |= _BV(REFS0);  // Voltage reference
	sbi(ADCSRA, ADEN);  // Enable ADC
}


/** Sample analog pin with 8-bit precision */
uint8_t adc_read_byte(uint8_t channel)
{
	write_low_nibble(ADMUX, channel);  // Select channel to sample
	sbi(ADMUX, ADLAR);  // Align result to left
	sbi(ADCSRA, ADSC);  // Start conversion

	while(bit_is_high(ADCSRA, ADSC));  // Wait for it...

	return ADCH;  // The upper 8 bits of ADC result
}


/** Sample analog pin with 10-bit precision */
uint16_t adc_read_word(uint8_t channel)
{
	write_low_nibble(ADMUX, channel);  // Select channel to sample
	cbi(ADMUX, ADLAR);  // Align result to right
	sbi(ADCSRA, ADSC);  // Start conversion

	while(get_bit(ADCSRA, ADSC));  // Wait for it...

	return ADCW;  // The whole ADC word (10 bits)
}
