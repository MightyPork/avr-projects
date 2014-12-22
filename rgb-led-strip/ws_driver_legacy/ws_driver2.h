#pragma once

#include "pins.h"

// -- Utility for driving WS2812B and similar RGB LED stripes --

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
#define WS_T_LATCH 6000
#endif


// More precise timing
// #define WS_T_1H  800
// #define WS_T_1L  450
// #define WS_T_0H  200
// #define WS_T_0L  650
// #define WS_T_LATCH 50000


// Only ws_show / ws_send_byte / ws_send_rgb should be used, with port alias macros
// See pins.h for reference

/** Latch and display the RGB values */
void ws_show_real(PORT_P port, BIT_N pin);
#define ws_show_aux(port, pin) ws_show_real(&reg_port(port), pin)
#define ws_show(io) ws_show_aux(io)


/** Send one byte to the RGB strip */
void ws_send_byte_real(PORT_P port, BIT_N pin, uint8_t b);
#define ws_send_byte_aux(port, pin, b) ws_send_byte_real(&reg_port(port), pin, b)
#define ws_send_byte(io, b) ws_send_byte_aux(io, b)


/** Send RGB color to the strip */
void ws_send_rgb_real(PORT_P port, BIT_N pin, uint8_t r, uint8_t g, uint8_t b);
#define ws_send_rgb_aux(port, pin, r, g, b) ws_send_rgb_real(&reg_port(port), pin, r, g, b)
#define ws_send_rgb(io, r, g, b) ws_send_rgb_aux(io, r, g, b)
