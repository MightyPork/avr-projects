//Imitace casovane bomby, Ondrej Hruska (c) 2010-2012
//-------------------------------------------------
// Zapojeni:
//                   +------u------+
//           reset --+ /RST    Vcc +-- napajeni +5V
//  Anoda jednotek --+ PD0     PB7 +-- segment D
//   Anoda desitek --+ PD1     PB6 +-- sedment E
//    (nezapojeno) --+ XT2     PB5 +-- segment C
//    (nezapojeno) --+ XT1     PB4 +-- segment H
//            drat --+ PD2     PB3 +-- segment G
//            drat --+ PD3     PB2 +-- segment A
//            drat --+ PD4     PB1 +-- segment F
//            drat --+ PD5     PB0 +-- segment B
//             GND --+ GND     PD6 +-- output signal (vybuch)
//                   +-------------+
//
// Nazvy segmentu na displeji:
//  ---A---
// |       |
// F       B
// |       |
//  ---G---
// |       |
// E       C
// |       |
//  ---D---   H
//

/*

Ports:

PORTB - segments

PD0 anode L
PD1 anode H
PD2 w0
PD3 w1
PD4 w2
PD5 w3
PD6 BOMB
PD7 -nc-


*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay_basic.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

void updateDisplayBuffer();
void ports_init();
void timer_init();
void multiplex(uint8_t times);
void boom();
void halt();
void init_animation();


/*FUSES =
{
	.low = 0xE4,
	.high = 0xDF
};*/


/** initial number of intervals */
#define INIT_MINS 60

/** seconds in one interval */
#define INIT_MAX 60


/** seconds in one interval */
#define RAPID_MAX 8

/** seconds in one interval */
#define RAPID_COUNT 5



/* MACROS */

/** set one hertz interrupt */
#define timer_enable() TIMSK=(1<<OCIE1A)

/** disable timer interrupt */
#define timer_disable() TIMSK=0



// segments DEChGAFB
#define B (1<<0)
#define F (1<<1)
#define A (1<<2)
#define G (1<<3)
#define H (1<<4)
#define C (1<<5)
#define E (1<<6)
#define D (1<<7)

// anodes
#define AL 1
#define AH 0

// wires
#define W0 2
#define W1 3
#define W2 4
#define W3 5

//bomb pin
#define BM 6

// pin groups
#define WIRES ((1<<W0)|(1<<W1)|(1<<W2)|(1<<W3))
#define ANODES  ((1<<AL)|(1<<AH))
#define BOMB (1<<BM)
#define SEGMENTS 0xFF


uint8_t num2seg[10];	// array for num -> 7seg translation

#define BLANK 0


/* VARIABLES */

uint8_t EEMEM wireset;

uint8_t volatile cnt;				// time counter (countdown)
uint8_t volatile cnt_interval;		// current interval counter in seconds
uint8_t volatile cnt_interval_max;	// length of current interval in seconds

uint8_t disp_H; 	// display buffer - ones
uint8_t disp_L; 	// tens

uint8_t volatile wires;
uint8_t volatile last_wires;
uint8_t is_one_digit;

uint8_t wire_shutdown;	//absolute mask
uint8_t wire_boom;
//remaining two are for RAPID COUNTDOWN (30 s)




/* INTERRUPT VECTORS */

/** one second interrupt */
ISR(TIMER1_COMPA_vect){

	cnt_interval++;

	if(!is_one_digit) disp_H ^= H;
	disp_L ^= H;

	if(cnt_interval >= cnt_interval_max){
		cnt_interval=0;
		cnt--;
		//to tens and ones
		updateDisplayBuffer();
	}

	//time over?
	if(cnt==0){
		boom();
	}
}

/** MAIN */
int main()
{
	cnt = INIT_MINS;
	cnt_interval = 0;
	cnt_interval_max = INIT_MAX;
	is_one_digit = 0;
	disp_H = 0;
	disp_L = 0;
	last_wires = WIRES;


	num2seg[0] = A|B|C|D|E|F;
	num2seg[1] = B|C;
	num2seg[2] = A|B|G|E|D;
	num2seg[3] = A|B|G|C|D;
	num2seg[4] = F|G|B|C;
	num2seg[5] = A|F|G|C|D;
	num2seg[6] = A|C|D|E|F|G;
	num2seg[7] = A|B|C;
	num2seg[8] = A|B|C|D|E|F|G;
	num2seg[9] = A|B|C|D|F|G;

	timer_disable();
	timer_init();
	timer_reset();

	ports_init();
	init_animation();

	last_wires = ~(PIND & WIRES);

	//wire set
	//read wireset number
	uint8_t wireset_r = eeprom_read_byte((uint8_t*)&wireset);
		//go to next one, reset if >11
		wireset_r++;
		if(wireset_r>=12) wireset_r=0;
		//store new value
	eeprom_write_byte((uint8_t*)&wireset,wireset_r);

	//select wires for wireset
	uint8_t ws_boom[12] 		= {(1<<W1), (1<<W0), (1<<W2), (1<<W0), (1<<W3), (1<<W0), (1<<W1), (1<<W1), (1<<W3), (1<<W2), (1<<W2), (1<<W3)};
	uint8_t ws_shutdown[12] 	= {(1<<W0), (1<<W3), (1<<W1), (1<<W2), (1<<W2), (1<<W1), (1<<W3), (1<<W2), (1<<W1), (1<<W0), (1<<W3), (1<<W0)};

	wire_boom = ws_boom[wireset_r];
	wire_shutdown = ws_shutdown[wireset_r];


	updateDisplayBuffer();
	timer_enable();
	sei();


	uint8_t diff;


	while(1){
		multiplex(50);
		//check wire
		wires = ~(PIND & WIRES);
		diff = wires ^ last_wires;

		//wire changed
		if(diff != 0){

			//shutdown wire was connected last time
			if( (last_wires & wire_shutdown) != 0 && (diff & wire_shutdown) != 0){
				//shutdown wire!
				halt();
				exit(0);

			}else if( (last_wires & wire_boom) != 0 && (diff & wire_boom) != 0){
				//bomb launched!
				boom();
				exit(0);

			}else{
				//rapid
				if(cnt_interval_max > RAPID_MAX){
					cnt_interval_max = RAPID_MAX;

					if(cnt>RAPID_COUNT){
						cnt = RAPID_COUNT;
						cnt_interval = 0;
						timer_reset();
						updateDisplayBuffer();
					}

				}
			}

		}
		last_wires = wires;
	}

	cli();
	return 0;
}




/** put corrent segments into display buffer (from cnt) */
void updateDisplayBuffer(){
	div_t foo = div((int)cnt,10);
	//keep decimal dots
	disp_H = (disp_L & H) | num2seg[foo.quot];
	disp_L = (disp_L & H) | num2seg[foo.rem];
	if(foo.quot == 0){
		disp_H = 0;
		is_one_digit = 1;
	}else{
		is_one_digit = 0;
	}
}


/** show display, repeat "times" x */
void multiplex(uint8_t times){
	for(; times>0; times--){

		PORTB = ~disp_L; 		// TENS segments; common anode, needs invert
		PORTD &= ~ANODES;		// reset anodes
		PORTD |= (1<<AL);		// turn TENS anode on
		_delay_loop_1(255);		// wait


		PORTB = ~disp_H; 		// ONES segments; common anode, needs invert
		PORTD &= ~ANODES;		// reset anodes
		PORTD |= (1<<AH);		// turn ONES anode on
		_delay_loop_1(255);		// wait

	}
	PORTB = ~BLANK; //display off
}

void multiplex_pwm(uint8_t times, uint8_t delay){
	for(; times>0; times--){

		PORTB = ~disp_L; 		// TENS segments; common anode, needs invert
		PORTD &= ~ANODES;		// reset anodes
		PORTD |= (1<<AL);		// turn TENS anode on
		_delay_loop_1(255-delay);		// wait


		PORTD &= ~(1<<AL);
		_delay_loop_1(delay);


		PORTB = ~disp_H; 		// ONES segments; common anode, needs invert
		PORTD &= ~ANODES;		// reset anodes
		PORTD |= (1<<AH);		// turn ONES anode on
		_delay_loop_1(255-delay);		// wait

		PORTD &= ~(1<<AH);
		_delay_loop_1(delay);

	}
	PORTB = ~BLANK; //display off
}


/** the boom */
void boom(){

	cli();

	PORTD |= BOMB;				//activate bomb
	PORTD &= ~WIRES;			// turn off wire pullups

	disp_H = 0b01010101;
	disp_L = 0b10101010;

	while(1){
		disp_H ^= 0xff;
		disp_L ^= 0xff;

		longmpx();
	}

//	PORTD |= ANODES;			// turn both anodes on
//	PORTB = 0x00;		 		// all segments on
//	__asm__("cli");

	exit(0);
}


/** the boom */
void halt(){



	PORTD &= ~WIRES;

	for(uint8_t volatile pause=0; pause < 0xff; pause++){
		multiplex_pwm(40,pause);
	}

	//animation

	PORTD = (1<<AL);			// higher part
	PORTB = ~H;					// show one dot

	timer_disable();
	cli();

	for(;;){}

	exit(0);
}

/** set up ports */
void ports_init(){

	DDRB = SEGMENTS;			// segments to output
	PORTB = SEGMENTS;			// turned off (cathodes!)

	DDRD = ANODES|BOMB;			// set outputs
	PORTD = WIRES;				// enable wire pullups

}


/** set timer1 to one hertz */
void timer_init(){
	TCCR1A = 0;
	TCCR1B=(1<<WGM12)|(1<<CS12);
	OCR1AH=0b01111010;
	OCR1AL=0b00010010;
}

/** clear timer */
void timer_reset(){
	TCNT0 = 0;
}

void longmpx(){
	for(uint8_t i=2; i>0; i--){
		multiplex(250);
	}
}

/** animation on startup */
void init_animation(){

	disp_H = BLANK;
	disp_L = BLANK;
	for(uint8_t i=0; i<10; i++){
		disp_H ^= 0xff;
		disp_L ^= 0xff;

		longmpx();
	}

	disp_H = BLANK;
	disp_L = BLANK;

	_delay_loop_1(255);
}
