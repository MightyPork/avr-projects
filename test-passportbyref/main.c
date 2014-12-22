#include <avr/io.h>

void led_on(volatile uint8_t* portp, uint8_t pinn) {
	*portp |= _BV(pinn);
}

void led_off(volatile uint8_t* portp, uint8_t pinn) {
	*portp &= ~ _BV(pinn);
}

void main()
{
	led_on(&PORTB, 3);
	led_off(&PORTB, 5);
	led_on(&DDRB, 3);
}
