#pragma once

/** Weird constructs for the compiler */

// general macros
#define SECTION(pos) __attribute__((naked, used, section(pos)))
