#pragma once

/**
  Utils for driving a WS2812 (WS2812B) RGB LED strips.

  It's implemented as macros to avoid overhead when passing values, and to
  enable driving multiple strips at once.

  To avoid bloating your code, try to reduce the number of invocations -
  compute color and then send it.

  [IMPORTANT]

  Some seemingly random influences can ruin the communication.
  If you have enough memory, consider preparing the colors in array,
  and sending this array using one of the "ws_send_XXX_array" macros.

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
#define ws_show() do {delay_ns_c(WS_T_LATCH, 0); } while(0)


/** Send one byte to the RGB strip */
#define ws_send_byte(io, bb) do {										\
	for (volatile int8_t __ws_tmp = 7; __ws_tmp >= 0; --__ws_tmp) {		\
		if ((bb) & (1 << __ws_tmp)) {									\
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

/** Send array of colors */
#define ws_send_xrgb_array(io, rgbs, length) __ws_send_array_proto(io_pack(io), (rgbs), (length), xrgb)
#define ws_send_rgb24_array(io, rgbs, length) __ws_send_array_proto(io_pack(io), (rgbs), (length), rgb24)
#define ws_send_rgb15_array(io, rgbs, length) __ws_send_array_proto(io_pack(io), (rgbs), (length), rgb15)
#define ws_send_rgb12_array(io, rgbs, length) __ws_send_array_proto(io_pack(io), (rgbs), (length), rgb12)
#define ws_send_rgb6_array(io, rgbs, length) __ws_send_array_proto(io_pack(io), (rgbs), (length), rgb6)

// prototype for sending array. it's ugly, sorry.
#define __ws_send_array_proto(io, rgbs, length, style) do {								\
	for (uint8_t __ws_tmp_sap_i = 0; __ws_tmp_sap_i < length; __ws_tmp_sap_i++) {		\
		style ## _t __ws_tmp_sap2 = (rgbs)[__ws_tmp_sap_i];								\
		ws_send_ ## style(io_pack(io), __ws_tmp_sap2);									\
	}																					\
} while(0)
