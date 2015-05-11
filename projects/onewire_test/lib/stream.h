#pragma once

//
// Streams -- in this library -- are instances of type STREAM.
//
// A stream can be used for receiving and sending bytes, generally
// it's a pipe to a device.
//
// They are designed for printing numbers and strings, but can
// also be used for general data transfer.
//
// Examples of streams:
// "uart.h" -> declares global variable "uart" which is a pointer to the UART stream
// "lcd.h" -> declares a global variable "lcd" (pointer to LCD scho stream)
//
// Streams help avoid code duplication, since the same functions can be used
// to format and print data to different device types.
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <avr/pgmspace.h>

/** Stream structure */
typedef struct {
	void (*tx) (uint8_t b);
	uint8_t (*rx) (void);
} STREAM;


/** Print string into a stream */
void put_str(const STREAM *p, char* str);


/** Print a programspace string into a stream */
void put_str_P(const STREAM *p, const char* str);


/** Send signed int8 */
#define put_char(p, c) (p)->tx((c))
void put_u8(const STREAM *p, const uint8_t num);


/** Send unsigned int8 */
void put_i8(const STREAM *p, const int8_t num);


/** Send unsigned int */
void put_u16(const STREAM *p, const uint16_t num);


/** Send signed int */
void put_i16(const STREAM *p, const int16_t num);


/** Send unsigned long */
void put_u32(const STREAM *p, const uint32_t num);


/** Send signed long */
void put_i32(const STREAM *p, const int32_t num);


/** Send unsigned int8 */
void put_x8(const STREAM *p, const uint8_t num);


/** Send int as hex */
void put_x16(const STREAM *p, const uint16_t num);


/** Send long as hex */
void put_x32(const STREAM *p, const uint32_t num);


/** Send long long as hex */
void put_x64(const STREAM *p, const uint64_t num);


// float variant doesn't make sense for 8-bit int

/** Send unsigned int as float */
void put_u16f(const STREAM *p, const uint16_t num, const uint8_t places);


/** Send signed int as float */
void put_i16f(const STREAM *p, const int16_t num, const uint8_t places);


/** Send unsigned long as float */
void put_u32f(const STREAM *p, const uint32_t num, const uint8_t places);


/** Send signed long as float */
void put_i32f(const STREAM *p, const int32_t num, const uint8_t places);


/** Print CR LF */
void put_nl(const STREAM *p);
