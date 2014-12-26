#pragma once

#define loop(varname, count) for(uint8_t varname = 0; varname < (count); varname++)
#define repeat(count) for(uint8_t _repeat_i = 0; _repeat_i < (count); _repeat_i++)
#define until(cond) while(!(cond))
