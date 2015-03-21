#pragma once

/*
  HSL support (addition to colors.h)
*/

#include "colors.h"

// Define HSL_LINEAR to get more linear brightness in hsl->rgb conversion
#ifdef HSL_LINEAR
# include "linear_fade.h"
#endif

// HSL data structure
typedef struct {
	uint8_t h;
	uint8_t s;
	uint8_t l;
} hsl_t;

/* Convert HSL to XRGB */
xrgb_t hsl2xrgb(const hsl_t color);
