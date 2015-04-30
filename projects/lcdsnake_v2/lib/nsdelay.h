#pragma once

//
// Functions for precise delays (nanoseconds / cycles)
//

#include <avr/io.h>
#include <util/delay_basic.h>
#include <stdint.h>

/* Convert nanoseconds to cycle count */
#define ns2cycles(ns)  ( (ns) / (1000000000L / (signed long) F_CPU) )

/** Wait c cycles */
#define delay_c(c)  (((c) > 0) ? __builtin_avr_delay_cycles(c) :  __builtin_avr_delay_cycles(0))

/** Wait n nanoseconds, plus c cycles  */
#define delay_ns_c(ns, c)  delay_c(ns2cycles(ns) + (c))

/** Wait n nanoseconds  */
#define delay_ns(ns)  delay_c(ns2cycles(ns))
