#include <stdlib.h>
#include <stdint.h>
#include "colors.h"
#include "hsl.h"

#ifdef HSL_LINEAR
	const uint8_t FADE_128[] = {
		0,	1,	1,	1,	1,	1,	2,	2,	2,	2,	2,	3,	3,	3,	4,	4,	4,	4,
		5,	5,	6,	6,	6,	7,	7,	8,	8,	8,	9,	10,	10,	10,	11,	12,	13,	14,
		14,	15,	16,	17,	18,	20,	21,	22,	24,	26,	27,	28,	30,	31,	32,	34,	35,	36,
		38,	39,	40,	41,	42,	44,	45,	46,	48,	49,	50,	52,	54,	56,	58,	59,	61,	63,
		65,	67,	68,	69,	71,	72,	74,	76,	78,	80,	82,	85,	88,	90,	92,	95,	98,	100,
		103, 106, 109, 112, 116, 119, 122, 125, 129, 134, 138, 142,	147, 151,
		153, 156, 160, 163, 165, 170, 175, 180, 185, 190, 195, 200, 207, 214, 218,
		221, 225, 228, 232, 234, 241, 248, 254, 255
	};
#endif

// based on: https://github.com/lewisd32/avr-hsl2rgb
xrgb_t hsl2xrgb(const hsl_t cc)
{
	// 0 .. 256*3
	const uint16_t hh = (uint16_t) cc.h * 3;
	const uint8_t hue_mod = hh % 256;

	uint8_t r_temp, g_temp, b_temp;
	if (hh < 256) {
		r_temp = hue_mod ^ 255;
		g_temp = hue_mod;
		b_temp = 0;
	} else if (hh < 512) {
		r_temp = 0;
		g_temp = hue_mod ^ 255;
		b_temp = hue_mod;
	} else if (hh < 768) {
		r_temp = hue_mod;
		g_temp = 0;
		b_temp = hue_mod ^ 255;
	} else {
		r_temp = 0;
		g_temp = 0;
		b_temp = 0;
	}

	const uint8_t inverse_sat = (cc.s ^ 255);

	xrgb_t rgb;

	uint8_t t8;
	uint16_t t16;

#ifdef HSL_LINEAR
	const uint8_t bri = FADE_128[cc.l>>1];
#else
	const uint8_t bri = cc.l;
#endif

	t8 = r_temp;
	t16 = t8 * cc.s + t8;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	t8 = t8 + inverse_sat;
	t16 = t8 * bri;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	rgb.r = t8;

	t8 = g_temp;
	t16 = t8 * cc.s;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	t8 = t8 + inverse_sat;
	t16 = t8 * bri;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	rgb.g = t8;

	t8 = b_temp;
	t16 = t8 * cc.s;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	t8 = t8 + inverse_sat;
	t16 = t8 * bri;
	t16 = t16 + t8;
	t8 = t16 >> 8;
	rgb.b = t8;

	return rgb;
}
