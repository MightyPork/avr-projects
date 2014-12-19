#pragma once

// general macros
#define SECTION(pos) __attribute__((naked, used, section(pos)))


// pin manipulation
#define sbi(port, bit) (port) |= _BV(bit)
#define cbi(port, bit) (port) &= ~ _BV(bit)

#define setBit(port, bit, value) (port) = (((port) & ~_BV(bit)) | ((value) << (bit)))
#define getBit(port, bit) (((port) >> (bit)) & 1)

#define busPulse(port, bit) { sbi(port, bit); cbi(port, bit); }
