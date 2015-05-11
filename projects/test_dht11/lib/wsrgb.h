#pragma once

//
// Utils for driving a WS2812 RGB LED strips, and color manipulation in general.
//
// Timing is critical!
//
// Create a config file rgb_config.h next to your main.c
//

#include "iopins.h"
#include "color.h"

// Your config file
#include "ws_config.h"

/*
	#define WS_T_1H  700
	#define WS_T_1L  150
	#define WS_T_0H  150
	#define WS_T_0L  700
	#define WS_T_LATCH 7000
	#define WS_PIN 2
*/


// --- functions for RGB strips ---

/** Initialize OI */
void ws_init();

/** Wait long enough for the colors to show */
void ws_show();

/** Send one byte to the RGB strip */
void ws_send_byte(const uint8_t bb);

/** Send R,G,B color to the strip */
void ws_send_rgb(const uint8_t r, const uint8_t g, const uint8_t b);

/** Send a RGB struct */
void ws_send_xrgb(xrgb_t xrgb);

/** Send color hex */
void ws_send_rgb24(rgb24_t rgb);

/** Send array of colors */
void ws_send_xrgb_array(const xrgb_t rgbs[], const uint8_t length);

/** Send array of colors */
void ws_send_rgb24_array(const rgb24_t rgbs[], const uint8_t length);
