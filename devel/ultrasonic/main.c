#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#include "lib/calc.h"
#include "lib/uart.h"
#include "lib/uart_ansi.h"
#include "lib/pins.h"
#include "lib/arduino_pins.h"
#include "lib/sonar.h"

ISR(PCINT0_vect)
{
	if (sonar_handle_pci()) return;
}

// All PCINT vectors must call sonar_handle_pci
// This way, they are linked to PCINT0
// so we can avoid code duplication
ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));


// Timer overflow - if sonar has a timeout -> 0xFFFF in result.
ISR(TIMER1_OVF_vect)
{
	if (sonar_handle_t1ovf()) return;
}


void main()
{
	uart_init(9600);
	vt_init();

	sonar_t so;
	sonar_t so2;

	sonar_init(&so, A0, A1);
	sonar_init(&so2, A2, A3);

	sei();

	// end
	int16_t res;
	while(1) {
		vt_clear();
		vt_home();
		uart_puts_pgm(PSTR("SONAR\r\n==================="));

		// first
		vt_goto(99, 99); // move cursor away
		sonar_start(&so);
		while(sonar_busy());
		res = sonar_result();

		// print value
		vt_goto(0, 2);
		uart_puts_pgm(PSTR("A: "));

		if (res < 0) {
			uart_puts_pgm(PSTR("No Obstacle"));
		} else {
			uart_puti(res, 1);
			uart_puts_pgm(PSTR(" cm"));
		}

		// second
		vt_goto(99, 99); // move cursor away
		sonar_start(&so2);
		while(sonar_busy());
		res = sonar_result();

		// print value
		vt_goto(0, 3);
		uart_puts_pgm(PSTR("B: "));

		if (res < 0) {
			uart_puts_pgm(PSTR("No Obstacle"));
		} else {
			uart_puti(res, 1);
			uart_puts_pgm(PSTR(" cm"));
		}

		vt_goto(99, 99); // move cursor away
		_delay_ms(200);
	}
}
