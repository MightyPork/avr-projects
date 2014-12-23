#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>

#include "lib/meta.h"
#include "lib/arduino_pins.h"
#include "lib/calc.h"
#include "lib/colors.h"
#include "lib/ws2812.h"

#define WS1 D10

void render();
void init_karts();
void move_karts();


void SECTION(".init8") init_io()
{
	// led strip data
	as_output(WS1);

	// setup timer 10 ms
	TCCR0A = _BV(WGM01); // CTC
	TCCR0B = _BV(CS02) | _BV(CS00);  // prescaler 512
	OCR0A = 100;  // interrupt every 10 ms 156
	sbi(TIMSK0, OCIE0A);
	sei();
}

/** timer 0 interrupt vector */
ISR(TIMER0_COMPA_vect)
{
	move_karts();
	render();
}


/** Unsigned int range struct */
typedef struct {
	uint8_t a;
	uint8_t b;
	int8_t dir;
	uint8_t steptime;
	uint8_t stepcnt;
	xrgb_t color;
} kart_t;

#define BLACK xrgb(0,0,0)

#define karts_len 6
#define screen_len 72
kart_t karts[karts_len];
xrgb_t screen[screen_len];


void init_karts()
{
	karts[0] = (kart_t) { .a=0,   .b=17,   .dir=1,  .steptime=2,   .stepcnt=0, .color=xrgb(50, 0, 0) };
	karts[1] = (kart_t) { .a=10,  .b=19,  .dir=1,  .steptime=3,   .stepcnt=0, .color=xrgb(0, 50, 0) };
	karts[2] = (kart_t) { .a=20,  .b=29,  .dir=1,  .steptime=7,   .stepcnt=0, .color=xrgb(0, 0, 50) };
	karts[3] = (kart_t) { .a=40,  .b=52,   .dir=-1, .steptime=2,  .stepcnt=0, .color=xrgb(50, 0, 50) };
	karts[4] = (kart_t) { .a=50,  .b=69,  .dir=-1,  .steptime=11,  .stepcnt=0, .color=xrgb(0, 20, 20) };
	karts[5] = (kart_t) { .a=25,  .b=37,  .dir=-1,  .steptime=1,  .stepcnt=0, .color=xrgb(60, 30, 0) };
	karts[5] = (kart_t) { .a=17,  .b=25,  .dir=1,  .steptime=5,  .stepcnt=0, .color=xrgb(25, 25, 0) };
}


void move_karts()
{
	for (uint8_t j = 0; j < karts_len; j++) {
		if (++karts[j].stepcnt >= karts[j].steptime) {
			// move yo ass

			if (karts[j].dir > 0) {
				inc_wrap(karts[j].a, 0, screen_len - 1);
				inc_wrap(karts[j].b, 0, screen_len - 1);
			} else {
				dec_wrap(karts[j].a, 0, screen_len - 1);
				dec_wrap(karts[j].b, 0, screen_len - 1);
			}

			karts[j].stepcnt = 0;
		}
	}
}


void render()
{
	// build the screen

	// for each pixel
	for (uint8_t i = 0; i < screen_len; i++) {
		screen[i] = BLACK;
		// for each kart
		for (uint8_t j = 0; j < karts_len; j++) {
			if (in_range_wrap(i, karts[j].a, karts[j].b)) {
				screen[i] = add_xrgb(screen[i], karts[j].color);
			}
		}
	}

	for (uint8_t i = 0; i < screen_len; i++) {
		ws_send_xrgb(WS1, screen[i]);
	}
}


void main()
{
	init_karts();
	while(1) {}  // Timer interrupts do the rest
}
