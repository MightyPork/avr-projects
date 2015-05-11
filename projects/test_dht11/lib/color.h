#pragma once

// --- color types ---
//
// The XXXc macros don't use cast, so they can be used in array initializers.
//
//  xrgb ... 3-byte true-color RGB (8 bits per component)
//  rgb24 ... 24-bit color value, with equal nr of bits per component
//
//  XX_r (_g, _b) ... extract component from the color, and convert it to 0..255

// Define HSL_LINEAR to get more linear brightness in hsl->rgb conversion


typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} xrgb_t;


typedef uint32_t rgb24_t;

#define xrgb(rr, gg, bb) ((xrgb_t)xrgbc(rr, gg, bb))
// xrgb for constant array declarations
#define xrgbc(rr, gg, bb) { .r = ((uint8_t)(rr)), .g = ((uint8_t)(gg)), .b = ((uint8_t)(bb)) }
#define xrgb_r(c) ((uint8_t)(c.r))
#define xrgb_g(c) ((uint8_t)(c.g))
#define xrgb_b(c) ((uint8_t)(c.b))
#define xrgb_rgb24(c) ((((rgb24_t)c.r) << 16) | (((rgb24_t)c.g) << 8) | (((rgb24_t)c.b)))
#define xrgb_rgb15(c) (((((rgb15_t)c.r) & 0xF8) << 7) | ((((rgb15_t)c.g) & 0xF8) << 2) | ((((rgb15_t)c.b) & 0xF8) >> 3))
#define xrgb_rgb12(c) (((((rgb12_t)c.r) & 0xF0) << 4) | ((((rgb12_t)c.g) & 0xF0)) | ((((rgb12_t)c.b) & 0xF0) >> 4))
#define xrgb_rgb6(c)  (((((rgb6_t)c.r) & 0xC0) >> 2)  | ((((rgb6_t)c.g) & 0xC0) >> 4) | ((((rgb6_t)c.b) & 0xC0) >> 6))

#define rgb24c(r,g,b) (((((rgb24_t)r) & 0xFF) << 16) | ((((rgb24_t)g) & 0xFF) << 8) | (((rgb24_t)b) & 0xFF))
#define rgb24(r,g,b) ((rgb24_t) rgb24(r,g,b))

#define rgb24_r(c) ((((rgb24_t) (c)) >> 16) & 0xFF)
#define rgb24_g(c) ((((rgb24_t) (c)) >> 8) & 0xFF)
#define rgb24_b(c) ((((rgb24_t) (c)) >> 0) & 0xFF)
#define rgb24_xrgb(c) xrgb(rgb24_r(c), rgb24_g(c), rgb24_b(c))
#define rgb24_xrgbc(c) xrgbc(rgb24_r(c), rgb24_g(c), rgb24_b(c))

#define add_xrgb(x, y) ((xrgb_t) { (((y).r > (255 - (x).r)) ? 255 : ((x).r + (y).r)), (((y).g > (255 - (x).g)) ? 255 : ((x).g + (y).g)), (((y).b > 255 - (x).b) ? 255 : ((x).b + (y).b)) })


// HSL data structure
typedef struct {
	uint8_t h;
	uint8_t s;
	uint8_t l;
} hsl_t;

/* Convert HSL to XRGB */
xrgb_t hsl_xrgb(const hsl_t color);
