#pragma once

/**
  Custom loops
*/

// Repeat code n times (uint8_t counter)
#define repeat(count) repeat_aux(count, _repeat_##__COUNTER__)
#define repeat_aux(count, cntvar) for (uint8_t cntvar = 0; cntvar < (count); cntvar++)

// Repeat code n times (uint16_t counter)
#define repeatx(count) repeatx_aux(count, _repeatx_##__COUNTER__)
#define repeatx_aux(count, cntvar) for (uint16_t cntvar = 0; cntvar < (count); cntvar++)

// Repeat with custom counter name (uint8_t)
#define loop(var, count) repeat_aux(count, var)
// ..., uint16_t
#define loopx(var, count) repeatx_aux(count, var)

// Do until condition is met
#define until(what) while(!(what))

