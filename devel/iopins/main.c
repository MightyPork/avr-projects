#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#include "lib/iopins.h"

void main()
{
	for (int i = 2; i <= 5; i++) {
		as_output_n(i);
	}

	as_input_pu(12);

	while(1) {
		for (int i = 2; i <= 5; i++) {
			if (is_low(12)) {
				set_high_n(i);
				_delay_ms(100);
				set_low_n(i);
			}
		}
	}
}
