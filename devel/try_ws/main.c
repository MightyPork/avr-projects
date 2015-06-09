#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#include "lib/iopins.h"
#include "lib/wsrgb.h"
#include "lib/uart.h"

void main()
{
	ws_init();

	xrgb_t c[3];
	c[0] = xrgb(255, 0, 255);
	c[1] = xrgb(0, 255, 255);
	c[2] = xrgb(255, 255, 0);

	while(1) {
		ws_send_xrgb_array(c, 3);

		ws_show();

		c[0].r++;
		c[1].b++;
		c[2].g++;

		_delay_ms(10);
	}
}
