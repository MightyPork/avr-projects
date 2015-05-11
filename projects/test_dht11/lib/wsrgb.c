#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "iopins.h"
#include "nsdelay.h"

#include "wsrgb.h"
#include "color.h"
#include "ws_config.h"


/* Driver code for WS2812B */

void ws_init()
{
	as_output(WS_PIN);
}

/** Wait long enough for the colors to show */
void ws_show() {
	delay_ns_c(WS_T_LATCH, 0);
}

/** Send one byte to the RGB strip */
void ws_send_byte(const uint8_t bb)
{
	for (volatile int8_t i = 7; i >= 0; --i) {
		if ((bb) & (1 << i)) {
			pin_high(WS_PIN);
			delay_ns_c(WS_T_1H, -2);

			pin_low(WS_PIN);
			delay_ns_c(WS_T_1L, -10);
		} else {
			pin_high(WS_PIN);
			delay_ns_c(WS_T_0H, -2);

			pin_low(WS_PIN);
			delay_ns_c(WS_T_0L, -10);
		}
	}
}


/** Send R,G,B color to the strip */
void ws_send_rgb(const uint8_t r, const uint8_t g, const uint8_t b)
{
	ws_send_byte(g);
	ws_send_byte(r);
	ws_send_byte(b);
}


/** Send a RGB struct */
void ws_send_xrgb(xrgb_t xrgb)
{
	ws_send_byte(xrgb.g);
	ws_send_byte(xrgb.r);
	ws_send_byte(xrgb.b);
}


/** Send color hex */
void ws_send_rgb24(rgb24_t rgb)
{
	ws_send_byte(rgb24_g(rgb));
	ws_send_byte(rgb24_r(rgb));
	ws_send_byte(rgb24_b(rgb));
}

/** Send array of colors */
void ws_send_xrgb_array(const xrgb_t rgbs[], const uint8_t length)
{
	for (uint8_t i = 0; i < length; i++) {
		const xrgb_t c = rgbs[i];
		ws_send_byte(c.g);
		ws_send_byte(c.r);
		ws_send_byte(c.b);
	}
}

/** Send array of colors */
void ws_send_rgb24_array(const rgb24_t rgbs[], const uint8_t length)
{
	for (uint8_t i = 0; i < length; i++) {
		const rgb24_t c = rgbs[i];
		ws_send_byte(rgb24_g(c));
		ws_send_byte(rgb24_r(c));
		ws_send_byte(rgb24_b(c));
	}
}

//#define ws_send_rgb24_array(rgbs, length) __ws_send_array_proto((rgbs), (length), rgb24)

// prototype for sending array. it's ugly, sorry.
/*#define __ws_send_array_proto(rgbs, length, style) {						\
	for (uint8_t __rgb_sap_i = 0; __rgb_sap_i < length; __rgb_sap_i++) {			\
		style ## _t __rgb_sap2 = (rgbs)[__rgb_sap_i];								\
		ws_send_ ## style(__rgb_sap2);								\
	}																			\
}*/


// /** Send a 2D array to a zig-zag display */
// #define ws_send_xrgb_array_zigzag(rgbs, width, height) {				\
// 	int8_t __rgb_sxaz_y, __rgb_sxaz_x;										\
// 	for(__rgb_sxaz_y = 0; __rgb_sxaz_y < (height); __rgb_sxaz_y ++) {			\
// 		for(__rgb_sxaz_x = 0; __rgb_sxaz_x < (width); __rgb_sxaz_x++) {		\
// 			ws_send_xrgb((rgbs)[__rgb_sxaz_y][__rgb_sxaz_x]);	\
// 		}																	\
// 		__rgb_sxaz_y++;														\
// 		for(__rgb_sxaz_x = (width) - 1; __rgb_sxaz_x >= 0; __rgb_sxaz_x--) {	\
// 			ws_send_xrgb((rgbs)[__rgb_sxaz_y][__rgb_sxaz_x]);	\
// 		}																	\
// 	}																		\
// }


// /* Send a linear array to a zig-zag display as a n*m board (row-by-row)
// #define ws_send_xrgb_array_zigzag_linear(rgbs, width, height) {					\
// 	int8_t __rgb_sxazl_x, __rgb_sxazl_y;													\
// 	for(__rgb_sxazl_y = 0; __rgb_sxazl_y < (height); __rgb_sxazl_y++) {					\
// 		for(__rgb_sxazl_x = 0; __rgb_sxazl_x < (width); __rgb_sxazl_x++) {					\
// 			ws_send_xrgb((rgbs)[__rgb_sxazl_y * (width) + __rgb_sxazl_x]);	\
// 		}																				\
// 		__rgb_sxazl_y++;																	\
// 		for(__rgb_sxazl_x = width-1; __rgb_sxazl_x >=0; __rgb_sxazl_x--) {					\
// 			ws_send_xrgb((rgbs)[__rgb_sxazl_y * (width) + __rgb_sxazl_x]);	\
// 		}																				\
// 	}																					\
// }
