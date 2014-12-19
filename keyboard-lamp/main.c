#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#include "utils.h"

/**
 *                   +--u--+
 *             RST --|     |-- Vcc
 *  SAVE LED : PB3 --| t13 |-- PB2 : BTN: HIGH
 *      N.C. : PB4 --|     |-- PB1 : BTN: LOW
 *             GND --|     |-- PB0 : LED
 *                   +-----+
 */


// pins config
#define PIN_LED PB0
#define PIN_LOW PB1
#define PIN_HIGH PB2
#define PIN_INDICATOR PB3

#define btn_low() (0 == get_bit(PINB, PIN_LOW))
#define btn_high() (0 == get_bit(PINB, PIN_HIGH))


/** Initialize IO ports. */
void SECTION(".init8") init_io(void)
{
	// set pin directions
	DDRB = _BV(PIN_LED) | _BV(PIN_INDICATOR);

	// pullups
	PORTB = _BV(PIN_LOW) | _BV(PIN_HIGH);

	// initialize the timer

	// Fast PWM mode, Output to OC0A
	OCR0A = 32;

	TCCR0A = _BV(WGM00) | _BV(WGM01) | _BV(COM0A1);
	TCCR0B = _BV(CS00);
}


#define BRIGHTNESS_LEN 121

const uint8_t BRIGHTNESS[] PROGMEM = {
	0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5,
	6, 6, 6, 7, 7, 8, 8, 8, 9, 10, 10, 10, 11, 12, 13, 14, 14,
	15, 16, 17, 18, 20, 21, 22, 24, 26, 27, 28, 30, 31, 32, 34,
	35, 36, 38, 39, 40, 41, 42, 44, 45, 46, 48, 49, 50, 52, 54,
	56, 58, 59, 61, 63, 65, 67, 69, 71, 72, 74, 76, 78, 80, 82,
	85, 88, 90, 92, 95, 98, 100, 103, 106, 109, 112, 116, 119,
	122, 125, 129, 134, 138, 142, 147, 151, 156, 160, 165, 170,
	175, 180, 185, 190, 195, 200, 207, 214, 221, 228, 234, 241,
	248, 255
};

#define apply_brightness(level) OCR0A = pgm_read_byte( & BRIGHTNESS[level] )


/**
 * Main function
 */
void main()
{
	bool changed = 0;
	bool changed_since_last_save = 0;

	uint8_t savetimer = 0;

	uint8_t level = eeprom_read_byte((uint8_t *) 0);

	if (level >= BRIGHTNESS_LEN)
		level = BRIGHTNESS_LEN - 1;
	else if (level == 0)
		level = BRIGHTNESS_LEN / 2;

	apply_brightness(level);

	while (1) {
		if (btn_high() && level < BRIGHTNESS_LEN - 1) {
			level++;
			changed = 1;
		}

		if (btn_low() && level > 1) {
			level--;
			changed = 1;
		}

		if (changed) {
			changed = 0;
			savetimer = 0;
			changed_since_last_save = 1;
			apply_brightness(level);
		}

		_delay_ms(20);
		savetimer++;

		if (savetimer >= 50 && changed_since_last_save) {
			changed_since_last_save = 0;
			savetimer = 0;

			sbi(PORTB, PIN_INDICATOR);
			eeprom_update_byte((uint8_t*) 0, level);
			_delay_ms(10);
			cbi(PORTB, PIN_INDICATOR);
		}
	}
}
