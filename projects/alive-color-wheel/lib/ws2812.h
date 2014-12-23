#pragma once

/**
  Utils for driving a WS2812 (WS2812B) RGB LED strips.

  It's implemented as macros to avoid overhead when passing values, and to
  enable driving multiple strips at once. There is over 1us of free time between
  the colors, which can be used for some processing or color computation.

  To avoid bloating your code, try to reduce the nuýmber of invocations -
  compute color and then send it.
*/

#include <avr/io.h>

#include "pins.h"
#include "nsdelay.h"
#include "colors.h"

/* Driver code for WS2812B */

// --- timing constraints (NS) ---

#ifndef WS_T_1H
# define WS_T_1H  700
#endif

#ifndef WS_T_1L
# define WS_T_1L  150
#endif

#ifndef WS_T_0H
# define WS_T_0H  150
#endif

#ifndef WS_T_0L
# define WS_T_0L  700
#endif

#ifndef WS_T_LATCH
# define WS_T_LATCH 7000
#endif


/** Wait long enough for the colors to show */
#define ws_show() do { delay_ns_c(WS_T_LATCH, 0); } while(0)


/** Send one byte to the RGB strip */
#define ws_send_byte(io, bb) do {										\
	for (volatile int8_t __wsba_i = 7; __wsba_i >= 0; --__wsba_i) {		\
		if ((bb) & (1 << __wsba_i)) {									\
			pin_high(io_pack(io)); delay_ns_c(WS_T_1H, -2);				\
			pin_low(io_pack(io)); delay_ns_c(WS_T_1L, -10);				\
		} else {														\
			pin_high(io_pack(io)); delay_ns_c(WS_T_0H, -2);				\
			pin_low(io_pack(io)); delay_ns_c(WS_T_0L, -10);				\
		}																\
	}																	\
} while(0)


/** Send R,G,B color to the strip */
#define ws_send_rgb(io, r, g, b) do {			\
	ws_send_byte(io_pack(io), g);				\
	ws_send_byte(io_pack(io), r);				\
	ws_send_byte(io_pack(io), b);				\
} while(0)

/** Send a RGB struct */
#define ws_send_xrgb(io, xrgb) ws_send_rgb(io_pack(io), (xrgb).r, (xrgb).g, (xrgb).b)

/** Send color hex */
#define ws_send_rgb24(io, rgb) ws_send_rgb(io_pack(io), rgb24_r(rgb), rgb24_g(rgb), rgb24_b(rgb))
#define ws_send_rgb15(io, rgb) ws_send_rgb(io_pack(io), rgb15_r(rgb), rgb15_g(rgb), rgb15_b(rgb))
#define ws_send_rgb12(io, rgb) ws_send_rgb(io_pack(io), rgb12_r(rgb), rgb12_g(rgb), rgb12_b(rgb))
#define ws_send_rgb6(io, rgb)  ws_send_rgb(io_pack(io), rgb6_r(rgb),  rgb6_g(rgb),  rgb6_b(rgb))
