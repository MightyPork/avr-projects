#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "lib/calc.h"

void main()
{
	printf("== calc.h ==\n");

	int i;
	int a;

	printf("fn: inc_wrap()\n");
	{
		// normal operation
		a = 1;
		inc_wrap(a, 0, 15);
		assert(a == 2);

		// overflow
		a = 14;
		inc_wrap(a, 0, 15);
		assert(a == 0);
	}


	printf("fn: inc_wrapi()\n");
	{
		// normal operation
		a = 1;
		inc_wrapi(a, 0, 15);
		assert(a == 2);

		// not overflow yet
		a = 14;
		inc_wrapi(a, 0, 15);
		assert(a == 15);

		// overflow
		a = 15;
		inc_wrap(a, 0, 15);
		assert(a == 0);
	}


	printf("fn: dec_wrap()\n");
	{
		// normal operation
		a = 5;
		dec_wrap(a, 0, 15);
		assert(a == 4);

		// underflow
		a = 0;
		dec_wrap(a, 0, 15);
		assert(a == 14);
	}


	printf("fn: dec_wrapi()\n");
	{
		// normal operation
		a = 5;
		dec_wrapi(a, 0, 15);
		assert(a == 4);

		// underflow
		a = 0;
		dec_wrapi(a, 0, 15);
		assert(a == 15);
	}


	printf("fn: sbi()\n");
	{
		a = 0;
		sbi(a, 2);
		assert(a == 0b100);
	}


	printf("fn: cbi()\n");
	{
		a = 0b11111;
		cbi(a, 2);
		assert(a == 0b11011);
	}


	printf("fn: read_bit()\n");
	{
		a = 0b101;
		assert(read_bit(a, 0) == 1);
		assert(read_bit(a, 1) == 0);
		assert(read_bit(a, 2) == 1);
	}


	printf("fn: bit_is_high()\n");
	{
		a = 0b101;
		assert(bit_is_high(a, 0) == 1);
		assert(bit_is_high(a, 1) == 0);
		assert(bit_is_high(a, 2) == 1);
	}


	printf("fn: bit_is_low()\n");
	{
		a = 0b101;
		assert(bit_is_low(a, 0) == 0);
		assert(bit_is_low(a, 1) == 1);
		assert(bit_is_low(a, 2) == 0);
	}


	printf("fn: toggle_bit()\n");
	{
		a = 0;
		toggle_bit(a, 2);
		assert(a == 0b00100);
		toggle_bit(a, 4);
		assert(a == 0b10100);
		toggle_bit(a, 4);
		assert(a == 0b00100);
	}

	// pointer variants

	int* b = &a;

	printf("fn: sbi_p()\n");
	{
		*b = 0;
		sbi_p(b, 2);
		assert(*b == 0b100);
	}


	printf("fn: cbi_p()\n");
	{
		*b = 0b11111;
		cbi_p(b, 2);
		assert(*b == 0b11011);
	}


	printf("fn: read_bit_p()\n");
	{
		*b = 0b101;
		assert(read_bit_p(b, 0) == 1);
		assert(read_bit_p(b, 1) == 0);
		assert(read_bit_p(b, 2) == 1);
	}


	printf("fn: bit_is_high_p()\n");
	{
		*b = 0b101;
		assert(bit_is_high_p(b, 0) == 1);
		assert(bit_is_high_p(b, 1) == 0);
		assert(bit_is_high_p(b, 2) == 1);
	}


	printf("fn: bit_is_low_p()\n");
	{
		*b = 0b101;
		assert(bit_is_low_p(b, 0) == 0);
		assert(bit_is_low_p(b, 1) == 1);
		assert(bit_is_low_p(b, 2) == 0);
	}


	printf("fn: toggle_bit_p()\n");
	{
		*b = 0;
		toggle_bit_p(b, 2);
		assert(*b == 0b00100);
		toggle_bit_p(b, 4);
		assert(*b == 0b10100);
		toggle_bit_p(b, 4);
		assert(*b == 0b00100);
	}


	// -- nibbles --

	printf("fn: write_low_nibble()\n");
	{
		a = 0xAC;
		write_low_nibble(a, 0x5); // shifted 4 left
		assert(a == 0xA5);

		a = 0xAB;
		write_low_nibble(a, 0x65); // shifted 4 left, extra ignored
		assert(a == 0xA5);
	}


	printf("fn: write_high_nibble()\n");
	{
		a = 0xAC;
		write_high_nibble(a, 0x5); // shifted 4 left
		assert(a == 0x5C);

		a = 0xAB;
		write_high_nibble(a, 0x65); // shifted 4 left, extra ignored
		assert(a == 0x5B);
	}


	printf("fn: write_low_nibble_p()\n");
	{
		*b = 0xAC;
		write_low_nibble_p(b, 0x5); // shifted 4 left
		assert(*b == 0xA5);

		*b = 0xAB;
		write_low_nibble_p(b, 0x65); // shifted 4 left, extra ignored
		assert(*b == 0xA5);
	}


	printf("fn: write_high_nibble_p()\n");
	{
		*b = 0xAC;
		write_high_nibble_p(b, 0x5); // shifted 4 left
		assert(*b == 0x5C);

		*b = 0xAB;
		write_high_nibble_p(b, 0x65); // shifted 4 left, extra ignored
		assert(*b == 0x5B);
	}


	printf("fn: in_range()\n");
	{
		// regular
		assert(in_range(10, 10, 15));
		assert(in_range(14, 10, 15));
		assert(in_range(13, 10, 15));
		// under
		assert(!in_range(9, 10, 15));
		assert(!in_range(0, 10, 15));
		// above
		assert(!in_range(15, 10, 15));
		assert(!in_range(99, 10, 15));

		// swapped bounds

		// regular
		assert(in_range(10, 15, 10));
		assert(in_range(14, 15, 10));
		assert(in_range(13, 15, 10));
		// under
		assert(!in_range(9, 15, 10));
		assert(!in_range(0, 15, 10));
		// above
		assert(!in_range(15, 15, 10));
		assert(!in_range(99, 15, 10));
	}


	printf("fn: in_rangei()\n");
	{
		// regular
		assert(in_rangei(10, 10, 15));
		assert(in_rangei(15, 10, 15));
		assert(in_rangei(13, 10, 15));
		// under
		assert(!in_rangei(9, 10, 15));
		assert(!in_rangei(0, 10, 15));
		// above
		assert(!in_rangei(16, 10, 15));
		assert(!in_rangei(99, 10, 15));

		// -- swapped bounds --

		// regular
		assert(in_rangei(10, 15, 10));
		assert(in_rangei(15, 15, 10));
		assert(in_rangei(13, 15, 10));
		// under
		assert(!in_rangei(9, 15, 10));
		assert(!in_rangei(0, 15, 10));
		// above
		assert(!in_rangei(16, 15, 10));
		assert(!in_rangei(99, 15, 10));
	}


	printf("fn: in_range_wrap()\n");
	{
		// as regular range

		// regular
		assert(in_range_wrap(10, 10, 15));
		assert(in_range_wrap(14, 10, 15));
		assert(in_range_wrap(13, 10, 15));
		// under
		assert(!in_range_wrap(9, 10, 15));
		assert(!in_range_wrap(0, 10, 15));
		// above
		assert(!in_range_wrap(15, 10, 15));
		assert(!in_range_wrap(16, 10, 15));
		assert(!in_range_wrap(99, 10, 15));

		// with wrap (>= 15, < 10)

		// regular
		assert(!in_range_wrap(10, 15, 10));
		assert(!in_range_wrap(14, 15, 10));
		assert(!in_range_wrap(13, 15, 10));
		// under
		assert(in_range_wrap(9, 15, 10));
		assert(in_range_wrap(0, 15, 10));
		// above
		assert(in_range_wrap(16, 15, 10));
		assert(in_range_wrap(99, 15, 10));
	}


	printf("fn: in_range_wrapi()\n");
	{
		// as regular range

		// regular
		assert(in_range_wrapi(10, 10, 15));
		assert(in_range_wrapi(15, 10, 15));
		assert(in_range_wrapi(13, 10, 15));
		// under
		assert(!in_range_wrapi(9, 10, 15));
		assert(!in_range_wrapi(0, 10, 15));
		// above
		assert(!in_range_wrapi(16, 10, 15));
		assert(!in_range_wrapi(99, 10, 15));

		// with wrap (>= 15, < 10)

		// regular
		assert(in_range_wrapi(10, 15, 10));
		assert(!in_range_wrapi(14, 15, 10));
		assert(!in_range_wrapi(13, 15, 10));
		// under
		assert(in_range_wrapi(10, 15, 10));
		assert(in_range_wrapi(0, 15, 10));
		// above
		assert(in_range_wrapi(16, 15, 10));
		assert(in_range_wrapi(99, 15, 10));
	}

	printf("== PASSED ==\n");
}
