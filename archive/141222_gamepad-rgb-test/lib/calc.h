#pragma once

/**
  General purpose calculation and bit manipulation utilities.
*/

// if max, go to zero. Else increment.
#define inc_wrap(var, max)  do { if ((var) >= (max)) { (var)=0; } else { (var)++; } } while(0)

// If zero, go to max. Else decrement,
#define dec_wrap(var, max)  do { if ((var) > 0) { (var)--; } else { (var)=(max); } } while(0)

// Check if value is in range A..B or B..A
#define in_range(x, low, high) (((low) < (high)) && ((x) > (low) && (x) < (high))) || (((low) > (high)) && ((x) < (low) || (x) > (high)))

// Check if value is in range A..B. If B < A, matches all outside B..A
#define in_range_wrap(x, low, high) (((low) < (high)) && ((x) > (low) && (x) < (high))) || (((low) > (high)) && ((x) > (low) || (x) < (high)))

// === general bit manipulation with register ===
#define sbi(reg, bit) do { (reg) |= (1 << (uint8_t)(bit)); } while(0)
#define cbi(reg, bit) do { (reg) &= ~(1 << (uint8_t)(bit)); } while(0)

#define read_bit(reg, bit) ((((uint8_t)(reg)) >> (uint8_t)(bit)) & 0x1)
#define get_bit(reg, bit) read_bit(reg, bit)

#define write_bit(reg, bit, value) do { (reg) = ((reg) & ~(1 << (uint8_t)(bit))) | (((uint8_t)(value) & 0x1) << (uint8_t)(bit)); } while(0)
#define set_bit(reg, bit, value) write_bit(reg, bit, value)
#define toggle_bit(reg, bit) do { (reg) ^= (1 << (uint8_t)(bit)); } while(0)

// general pin manipulation - with pointer to register
#define sbi_p(reg_p, bit) do { (*(reg_p)) |= (1 << (uint8_t)(bit)); } while(0)
#define cbi_p(reg_p, bit) do { (*(reg_p)) &= ~(1 << (uint8_t)(bit)); } while(0)

#define read_bit_p(reg_p, bit) ((*(reg_p) >> (uint8_t)(bit)) & 0x1)
#define get_bit_p(reg_p, bit) read_bit_p(reg_p, bit)

#define write_bit_p(reg_p, bit, value) do { *(reg_p) = (*(reg_p) & ~(1 << ((uint8_t)(bit) & 0x1))) | (((uint8_t)(value) & 0x1) << (uint8_t)(bit)); } while(0)
#define set_bit_p(reg, bit, value) write_bit_p(reg_p, bit, value)
#define toggle_bit_p(reg_p, bit) do { *(reg_p) ^= (1 << (uint8_t)(bit)); } while(0)

