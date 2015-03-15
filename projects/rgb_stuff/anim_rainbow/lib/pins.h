#pragma once

/**
  This file provides macros for pin manipulation.

  You can define your application pins like so:

      // Led at PORTB, pin 1
      #define LED B,1

      // Switch at PORTD, pin 7
      #define SW1 D,7

  Now you can use macros from this file to wirh with the pins, eg:

      as_output(LED);
      as_input(SW1);
      pullup_on(SW1);

      toggle_pin(LED);
      while (pin_is_low(SW1));

  - The macros io2XXX() can be used to get literal name of register associated with the pin.
    io2n() provides pin number.
  - The XXX_aux() macros are internal and should not be used elsewhere.
  - The io_pack() macro is used to pass pin (io) to other macro without expanding it.

  Additionaly, there's general-purpose bit-manipulation macros (set_bit, clear_bit etc.).
  Those work with register name and pin number, not the "io" format (#define LED2 D,3).

*/

#include <avr/io.h>
#include "calc.h"


// Get particular register associated with the name X (eg. D -> PORTD)
#define reg_ddr(X) DDR ## X
#define reg_port(X) PORT ## X
#define reg_pin(X) PIN ## X

#define io2ddr_aux(reg, bit) reg_ddr(reg)
#define io2ddr(io) io2ddr_aux(io)
#define io2port_aux(reg, bit) reg_port(reg)
#define io2port(io) io2port_aux(io)
#define io2pin_aux(reg, bit) reg_pin(reg)
#define io2pin(io) io2pin_aux(io)
#define io2n_aux(reg, bit) bit
#define io2n(io) io2n_aux(io)

#define io_pack(port, bit) port, bit


// pointer to port
typedef volatile uint8_t* PORT_P;
// number of bit in port
typedef uint8_t BIT_N;


// === pin manipulation ===
#define set_pin_aux(port, bit) set_bit(reg_port(port), bit)
#define clear_pin_aux(port, bit) clear_bit(reg_port(port), bit)
#define read_pin_aux(port, bit) read_bit(reg_pin(port), bit)
#define write_pin_aux(port, bit, value) write_bit(reg_port(port), bit, value)
#define toggle_pin_aux(port, bit) set_bit(reg_pin(port), bit)


#define set_pin(io) set_pin_aux(io)
#define pin_high(io) set_pin_aux(io)

#define clear_pin(io) clear_pin_aux(io)
#define pin_low(io) clear_pin_aux(io)

#define read_pin(io) read_pin_aux(io)

#define pin_is_low(io) !read_pin_aux(io)
#define pin_is_high(io) read_pin_aux(io)

#define write_pin(io, value) write_pin_aux(io, value)

#define toggle_pin(io) toggle_pin_aux(io)



// setting pin direction
#define as_input_aux(port, bit) clear_bit(reg_ddr(port), bit)
#define as_output_aux(port, bit) set_bit(reg_ddr(port), bit)
#define set_dir_aux(port, bit, dir) write_bit(reg_ddr(port), bit, dir)


#define as_input(io) as_input_aux(io)
#define as_output(io) as_output_aux(io)
#define set_dir(io, dir) set_dir_aux(io, dir)



// setting pullup
#define pullup_enable_aux(port, bit) set_bit(reg_port(port), bit)
#define pullup_disable_aux(port, bit) clear_bit(reg_port(port), bit)
#define set_pullup_aux(port, bit, on) write_bit(reg_port(port), bit, on)


#define pullup_enable(io) pullup_enable_aux(io)
#define pullup_on(io) pullup_enable_aux(io)

#define pullup_disable(io) pullup_disable_aux(io)
#define pullup_off(io) pullup_disable_aux(io)

#define set_pullup(io, on) set_pullup_aux(io, on)
