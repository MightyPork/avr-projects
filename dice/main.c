#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

/**
 *  Fuses are defined in makefile.
 *
 *  Led segments:
 *
 *    (DIAG1)         (DIAG2)
 *    (HORIZ)  (DOT)  (HORIZ)
 *    (DIAG2)         (DIAG1)
 *
 *
 *  Wiring:
 *
 *                   +--u--+
 *             RST --|     |-- Vcc
 * SEG_DIAG2 : PB3 --| t13 |-- PB2 : SEG_DIAG1
 * SEG_HORIZ : PB4 --|     |-- PB1 : SEG_DOT
 *             GND --|     |-- PB0 : BUTTON
 *                   +-----+
 *
 *     SEG: PIN -> LED -> RESISTOR -> GND
 *     BTN: PIN -> BUTTON -> GND
 *
 */


// general macros
#define SECTION(pos) __attribute__((naked, used, section(pos)))

#define true 1
#define false 0
typedef uint8_t bool;



// individual segment pins
#define SEG_DOT		_BV(PB1)
#define SEG_DIAG1	_BV(PB2)
#define SEG_DIAG2	_BV(PB3)
#define SEG_HORIZ	_BV(PB4)

// button pin
#define PIN_BUTTON	_BV(PINB0)

#define OUT_PINS	(SEG_DOT | SEG_DIAG1 | SEG_DIAG2 | SEG_HORIZ)
#define IN_PINS		(BUTTON)

// test if button is down
#define BUTTON_DOWN()	((PINB & PIN_BUTTON) == 0)


void init_io() SECTION(".init8");


/**
 * Initialize IO ports.
 */
void init_io()
{
	DDRB	= OUT_PINS;
	PORTB	= PIN_BUTTON; // pullup
}


/**
 * Consume interrupt used to wake from sleep
 */
ISR(PCINT0_vect)
{
	// do nothing
}


/**
 * Show dice segments.
 *
 * @param segments to show
 */
void show(uint8_t bits)
{
	// PIN -> LED -> GND
	PORTB = (PORTB & ~OUT_PINS) | (bits & OUT_PINS);

	// VCC -> LED -> PIN
	//PORTB = (PORTB & ~OUT_PINS) | ~(bits & OUT_PINS);
}


/**
 * Show number using segments.
 * 0=one, 5=six.
 *
 * @param number to show. MUST BE IN RANGE 0..5
 */
void show_number(const uint8_t number)
{
	if(number > 5) return;

	static const uint8_t num2seg[] = {
		/*1*/ SEG_DOT,
		/*2*/ SEG_DIAG1,
		/*3*/ SEG_DIAG2 | SEG_DOT,
		/*4*/ SEG_DIAG1 | SEG_DIAG2,
		/*5*/ SEG_DIAG1 | SEG_DIAG2 | SEG_DOT,
		/*6*/ SEG_DIAG1 | SEG_DIAG2 | SEG_HORIZ,
	};

	show( num2seg[number] );
}


/**
 * Enter power down mode and wait for pin change to wake up.
 */
void go_sleep()
{
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	cli();

	GIMSK = _BV(PCIE);
	PCMSK = _BV(PCINT0);

	sleep_enable();
	sei();
	sleep_cpu();
	cli();
	sleep_disable();

	GIMSK = 0;
	PCMSK = 0;

	sei();
}


// Delay between faces in slowing roll,
// after which the dice is stopped
#define DELAY_MS_FOR_STOP 600


// How long until dice enters sleep mode
#define IDLE_MS_FOR_SLEEP 5000


/**
 * Main function
 */
void main()
{
	//  == VARS ==

	// Used to randomize when button goes down
	// Needed to prevent cheating and add more randomness
	uint8_t entropy = 0;

	// Number shown on dice (zero-based, 0 ... one dot)
	uint8_t number = 0;

	// Delay between face flip in roll
	uint16_t delay_ms = DELAY_MS_FOR_STOP;

	// Added to delay_ms each flip.
	// Is increased to make slowing non-linear
	uint16_t plus = 1;

	// counts milliseconds when the dice was idle
	// used to trigger sleep mode
	uint16_t idle_counter = 0;

	// flag that dice is either spinning or slowing down
	bool rolling = false;


	// == BRING IT ON! ==

	// start by sleeping (avoid showing one default number)
	go_sleep();

	show_number(number);

	while(1) {
		// increment entropy counter
		entropy++;
		if(entropy > 5) entropy -= 6;

		// grab button state
		bool down = BUTTON_DOWN();

		if(!down && !rolling) {

			// dice idle

			// increment idle counter
			_delay_ms(5);
			idle_counter += 5;

			if(idle_counter > IDLE_MS_FOR_SLEEP) {
				// go sleep

				show(0);

				go_sleep();

				show_number(number);

				idle_counter = 0;
			}

			continue;

		} else {
			// button down, or rolling
			// reset idle counter
			idle_counter = 0;
		}


		if(down) {

			// button pressed

			// start rolling
			delay_ms = 1;
			plus = 1;

			if(!rolling) {
				// was not rolling before
				// apply entropy to number
				number = entropy;
				rolling = true;
			}

			_delay_ms(10); // delay to make iterating visible

		} else {

			// button not pressed

			if(delay_ms >= DELAY_MS_FOR_STOP) {
				// too slow flip, stop the rolling

				rolling = false;

				// blink
				for(uint8_t i=0; i<4; i++) {
					show(0);
					_delay_ms(40);
					show_number(number);
					_delay_ms(40);
				}

				continue;
			}

			// slow down a bit
			delay_ms += plus;
			plus += 6;
		}


		// advance to next number
		number++;
		if(number > 5) number -= 6;

		show_number(number);

		// delay in rolling
		for(uint16_t i=0; i < delay_ms; i++) {
			_delay_ms(1);

			// button pressed? Cancel delay.
			if(BUTTON_DOWN())
				break;
		}
	}
}
