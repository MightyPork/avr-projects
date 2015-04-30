#pragma once

//
//  This file provides macros for pin manipulation.
//
//  You can define your application pins like so:
//
//      // Led at PORTB, pin 1
//      #define LED B,1
//
//      // Switch at PORTD, pin 7
//      #define SW1 D,7
//
//  Now you can use macros from this file to wirh with the pins, eg:
//
//      as_output(LED);
//      as_input(SW1);
//      pullup_on(SW1);
//
//      toggle_pin(LED);
//      while (pin_is_low(SW1));
//
//  - The macros io2XXX() can be used to get literal name of register associated with the pin.
//  - io2n() provides pin number.
//  - The underscored and _aux macros are internal and should not be used elsewhere.
//  - The io_pack() macro is used to pass pin (io) to other macro without expanding it.
//

#include <avr/io.h>
#include "calc.h"

// Helpers
// Get particular register associated with the name X (eg. D -> PORTD)
#define _reg_ddr(X) DDR ## X
#define _reg_port(X) PORT ## X
#define _reg_pin(X) PIN ## X
#define _io2ddr_aux(reg, bit) _reg_ddr(reg)
#define _io2port_aux(reg, bit) _reg_port(reg)
#define _io2pin_aux(reg, bit) _reg_pin(reg)
#define _io2n_aux(reg, bit) bit


// === Convert A,1 to corresponding register and pin number ===

#define io2ddr(io) _io2ddr_aux(io)
#define io2port(io) _io2port_aux(io)
#define io2pin(io) _io2pin_aux(io)
#define io2n(io) _io2n_aux(io)

// === covert "A", "1" to "A,1" for passing on to another macro ===
#define io_pack(port, bit) port, bit


// === Useful types for ports and pins ===

// pointer to port
typedef volatile uint8_t* PORT_P;
// number of bit in port
typedef uint8_t BIT_N;


// === pin manipulation ===

// Helpers
#define _set_pin_aux(port, bit) sbi(_reg_port(port), (bit))
#define _clear_pin_aux(port, bit) cbi(_reg_port(port), (bit))
#define _read_pin_aux(port, bit) get_bit(_reg_pin(port), (bit))
#define _write_pin_aux(port, bit, value) set_bit(_reg_port(port), (bit), (value))
#define _toggle_pin_aux(port, bit) sbi(_reg_pin(port), (bit))


// Set pin to HIGH
#define pin_up(io) _set_pin_aux(io)
#define pin_high(io) _set_pin_aux(io)

// Set pin to LOW
#define pin_down(io) _clear_pin_aux(io)
#define pin_low(io) _clear_pin_aux(io)

// Get input pin value
#define get_pin(io) _read_pin_aux(io)
#define read_pin(io) _read_pin_aux(io)

// Check if pin is low or high
#define pin_is_low(io) !_read_pin_aux(io)
#define pin_is_high(io) _read_pin_aux(io)

// Write a value to pin
#define set_pin(io, value) _write_pin_aux(io, (value))
#define write_pin(io, value) _write_pin_aux(io, (value))
#define toggle_pin(io) _toggle_pin_aux(io)


// === Setting pin direction ===

// Helpers
#define _as_input_aux(port, bit) cbi(_reg_ddr(port), (bit))
#define _as_output_aux(port, bit) sbi(_reg_ddr(port), (bit))
#define _set_dir_aux(port, bit, dir) write_bit(_reg_ddr(port), (bit), (dir))


// Pin as input (_pu ... with pull-up)
#define as_input(io) _as_input_aux(io)
#define as_input_pu(io) do { _as_input_aux(io); _pullup_enable_aux(io); } while(0)

// Pin as output
#define as_output(io) _as_output_aux(io)

// Set direction (1 ... output)
#define set_dir(io, dir) _set_dir_aux(io, (dir))


// === Setting pullup ===

// Helpers
#define _pullup_enable_aux(port, bit) sbi(_reg_port(port), (bit))
#define _pullup_disable_aux(port, bit) cbi(_reg_port(port), (bit))
#define _set_pullup_aux(port, bit, on) write_bit(_reg_port(port), (bit), (on))

// Enable pullup
#define pullup_enable(io) _pullup_enable_aux(io)
#define pullup_on(io) _pullup_enable_aux(io)

// Disable pullup
#define pullup_disable(io) _pullup_disable_aux(io)
#define pullup_off(io) _pullup_disable_aux(io)

// Set pullup to value (1 ... pullup enabled)
#define set_pullup(io, on) _set_pullup_aux(io, on)
