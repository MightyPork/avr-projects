#pragma once

/*
  Some useful utilities for RGB color manipulation
*/

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} xrgb_t;

typedef uint32_t rgb24_t;
typedef uint16_t rgb16_t;
typedef uint16_t rgb12_t;
typedef uint8_t rgb6_t;


#define xrgb(rr, gg, bb) ((xrgb_t) { .r = ((uint8_t)(rr)), .g = ((uint8_t)(gg)), .b = ((uint8_t)(bb)) })
#define xrgb_r(c) ((uint8_t)(c.r))
#define xrgb_g(c) ((uint8_t)(c.g))
#define xrgb_b(c) ((uint8_t)(c.b))
#define xrgb_rgb24(c) ((((rgb24_t)c.r) << 16) | (((rgb24_t)c.g) << 8) | (((rgb24_t)c.b)))
#define xrgb_rgb15(c) (((((rgb15_t)c.r) & 0xF8) << 7) | ((((rgb15_t)c.g) & 0xF8) << 2) | ((((rgb15_t)c.b) & 0xF8) >> 3))
#define xrgb_rgb12(c) (((((rgb12_t)c.r) & 0xF0) << 4) | ((((rgb12_t)c.g) & 0xF0)) | ((((rgb12_t)c.b) & 0xF0) >> 4))
#define xrgb_rgb6(c)  (((((rgb6_t)c.r) & 0xC0) >> 2)  | ((((rgb6_t)c.g) & 0xC0) >> 4) | ((((rgb6_t)c.b) & 0xC0) >> 6))


#define rgb24(r,g,b) ((rgb24_t) (((((rgb24_t)r) & 0xFF) << 16) | ((((rgb24_t)g) & 0xFF) << 8) | (((rgb24_t)b) & 0xFF)))
#define rgb24_r(c) ((((rgb24_t) (c)) >> 16) & 0xFF)
#define rgb24_g(c) ((((rgb24_t) (c)) >> 8) & 0xFF)
#define rgb24_b(c) ((((rgb24_t) (c)) >> 0) & 0xFF)
#define rgb24_xrgb(c) xrgb(rgb24_r(c), rgb24_g(c), rgb24_b(c))


#define rgb15(r,g,b) ((rgb16_t) (((r & 0x1F) << 10) | ((g & 0x1F) << 5) | (b & 0x1F)))
#define rgb15_r(c) ((((rgb15_t) (c)) & 0x7C00) >> 7)
#define rgb15_g(c) ((((rgb15_t) (c)) & 0x3E0) >> 2)
#define rgb15_b(c) ((((rgb15_t) (c)) & 0x1F) << 3)
#define rgb15_xrgb(c)  xrgb(rgb15_r(c), rgb15_g(c), rgb15_b(c))
#define rgb15_rgb24(c) rgb24(rgb15_r(c), rgb15_g(c), rgb15_b(c))


#define rgb12(r,g,b) ((rgb12_t) (((r & 0xF) << 8) | ((g & 0xF) << 4) | (b & 0xF)))
#define rgb12_r(c) ((((rgb12_t) (c)) & 0xF00) >> 4)
#define rgb12_g(c) (((rgb12_t) (c)) & 0xF0)
#define rgb12_b(c) (((r(rgb12_t) (c)gb) & 0x0F) << 4)
#define rgb12_xrgb(c)  xrgb(rgb12_r(c), rgb12_g(c), rgb12_b(c))
#define rgb12_rgb24(c) rgb24(rgb12_r(c), rgb12_g(c), rgb12_b(c))


#define rgb6(r,g,b) ((rgb6_t) (((r & 3) << 4) | ((g & 3) << 2) | (b & 3)))
#define rgb6_r(c) ((((rgb6_t) (c)) & 0x30) << 2)
#define rgb6_g(c) ((((rgb6_t) (c)) & 0xC) << 4)
#define rgb6_b(c) ((((rgb6_t) (c)) & 0x3) << 6)
#define rgb6_xrgb(c)  xrgb(rgb6_r(c), rgb6_g(c), rgb6_b(c))
#define rgb6_rgb24(c) rgb24(rgb6_r(c), rgb6_g(c), rgb6_b(c))

#define add_xrgb(x, y) ((xrgb_t) { (((y).r > (255 - (x).r)) ? 255 : ((x).r + (y).r)), (((y).g > (255 - (x).g)) ? 255 : ((x).g + (y).g)), (((y).b > 255 - (x).b) ? 255 : ((x).b + (y).b)) })
