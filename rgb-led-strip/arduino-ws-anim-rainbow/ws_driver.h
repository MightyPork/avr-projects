#pragma once

// -- AVR GCC utility for driving WS2812B and similar RGB LED stripes --

// You must define the foillowing in config file or here:
// *  WS_PORT
// *  WS_BIT

#include "config.h"

// The pin must be set to OUTPUT before using the output functions


// --- timing constraints (NS) ---

#ifndef WS_T_1H
#define WS_T_1H  700
#endif

#ifndef WS_T_1L
#define WS_T_1L  150
#endif

#ifndef WS_T_0H
#define WS_T_0H  150
#endif

#ifndef WS_T_0L
#define WS_T_0L  700
#endif

#ifndef WS_T_LATCH
#define WS_T_LATCH 7000
#endif


// More accurate timing
// #define WS_T_1H  800
// #define WS_T_1L  450
// #define WS_T_0H  200
// #define WS_T_0L  650
// #define WS_T_LATCH 50000


/** Latch and display the RGB values */
void ws_show(void);

/** Send one byte to the RGB strip */
void ws_send_byte(uint8_t b);

/** Send RGB color to the strip */
void ws_send_rgb(uint8_t r, uint8_t g, uint8_t b);
