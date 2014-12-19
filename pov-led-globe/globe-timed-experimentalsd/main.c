#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include <math.h>

#include "utils.h"

// Change to the image you want:
#include "image_chk3264numbers.h"

#define DEBOUNCE_MS 3


/**
 *                   +--u--+
 *             RST --|     |-- Vcc
 *   TRIGGER : PB3 --| t13 |-- PB2 : CLOCK   ->
 *      N.C. : PB4 --|     |-- PB1 : STROBE  -> to chained 74HC4094's
 *             GND --|     |-- PB0 : DATA    ->
 *                   +-----+
 *
 * TRIGGER is connected to ground by spring switch
 * that gets activated at start of swing to right.
 * (It's a metal contact with weight)
 *
 * LEDs are connected to 74HC4094 outputs by anodes.
 * To use cathodes, invert byte in send_byte function.
 */



// pins config
#define PIN_DATA	PB0
#define PIN_STROBE	PB1
#define PIN_CLOCK	PB2
#define PIN_TRIGGER	PB3


#define trigger_on() (0 == get_bit(PINB, PIN_TRIGGER))



/** Measure counter 10 usec */
volatile uint16_t measure_counter;

volatile uint8_t waiting_for_trigger = 1;

/** Computed pixel timing */
uint16_t t_light = 50;  // 5 ms
uint16_t t_gap = 10;  // 1 ms


void measure_start(void);
void measure_stop(void);
void send_byte(uint8_t);
void display(void);
int main(void);

/** Initialize IO ports. */
// void init_io(void) ;

void SECTION(".init8") init_io(void)
{
	// set output pins
	DDRB = _BV(PIN_DATA) | _BV(PIN_STROBE) | _BV(PIN_CLOCK);

	// pullups
	PORTB = _BV(PIN_TRIGGER);

	// initialize the timer

	TCCR0B = _BV(CS00);  // prescaler 0
	OCR0A = 96;  // interrupt every 10 us
	sbi(TIMSK0, OCIE0A);

	// initialize external interrupt

	PCMSK = _BV(PCINT3);
	sbi(GIMSK, PCIE);  // pin change interrupt enable
}


/** timer 0 interrupt vector */
ISR(PCINT0_vect)
{
	if (waiting_for_trigger && trigger_on()) {
		measure_stop();
	}
}


/** timer 0 interrupt vector */
ISR(TIM0_COMPA_vect)
{
	// increment only if not too high
	if (waiting_for_trigger && measure_counter < 0xFFFF) {
		measure_counter++;
	}
}



/** Start counting time of interval */
void
measure_start()
{
	measure_counter = 0;  // reset the counter
	waiting_for_trigger = 1;  // start waiting for trigger -> stop.

	  // enable CompMatch A interrupt
}


/** Stop measuring, compute timing values */
void
measure_stop()
{
	uint16_t prec = (measure_counter / (COLS));
	t_light = ((prec*7)/100);
	t_gap = ((prec*3)/100);

	// if (t_light == 0) t_light=25;
	// if (t_gap == 0) t_gap=5;

	waiting_for_trigger = 0;  // stop waiting for trigger (new loop may start)
}





/** send one byte into registers */
void
send_byte(uint8_t b)
{
	for (uint8_t i=0; i<8; i++) {
		set_bit(PORTB, PIN_DATA, get_bit(b, i));

		bus_pulse(PORTB, PIN_CLOCK);  // strobe
	}
}


/** send STROBE pulse */
void
display()
{
	bus_pulse(PORTB, PIN_STROBE);
}


/**
 * Main function
 */
int
main()
{
	sei();

	while (1) {

		// while (waiting_for_trigger); // wait for interrupt on TRIGGER
		// _delay_ms(DEBOUNCE_MS);  // debounce
		// while (trigger_on());  // wait for trigger to turn off
		// _delay_ms(DEBOUNCE_MS);  // debounce
		// measure_start();

		for(uint8_t i = DEBOUNCE_MS; i > 0; i--) {
			while (trigger_on());
			_delay_ms(1);
		}
		measure_start();

		for (uint8_t col = 0; col < COLS; col++) {

			if(trigger_on()) break; // emergency exit

			// +++ light +++

			// send light values
			for(uint8_t row = ROWS; row > 0; row--) {
				send_byte(pgm_read_byte(&image[col][row-1]));
			}

			// send STROBE
			display();

			// Wait
			for(uint16_t i = t_light; i > 0; i--) {
				_delay_us(100);
			}


			// +++ dark +++

			// send zeros
			for(uint8_t row = ROWS; row > 0; row--) {
				send_byte(0);
			}

			// send STROBE
			display();

			// Wait
			for(uint16_t i = t_gap; i > 0; i--) {
				_delay_us(100);
			}
		}
	}
}
