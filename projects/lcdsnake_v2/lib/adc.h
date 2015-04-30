#pragma once

//
// Utilities for build-in A/D converter
//

#include <avr/io.h>

/** Initialize the ADC */
void adc_init();

/** Disable AD (for power saving?) */
void adc_disable();

/** Sample analog pin with 8-bit precision */
uint8_t adc_read_byte(uint8_t channel);

/** Sample analog pin with 10-bit precision */
uint16_t adc_read_word(uint8_t channel);
