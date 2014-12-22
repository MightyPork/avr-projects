#pragma once

// general macros
#define SECTION(pos) __attribute__((naked, used, section(pos)))


// pin manipulation
#define sbi(port, bit) (port) |= _BV(bit)
#define cbi(port, bit) (port) &= ~ _BV(bit)

#define set_bit(port, bit, value) (port) = (((port) & ~_BV(bit)) | ((value) << (bit)))
#define get_bit(port, bit) (((port) >> (bit)) & 1)

#define bus_pulse(port, bit) { sbi(port, bit); cbi(port, bit); }
