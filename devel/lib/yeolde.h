#pragma once

/**
  Ye Olde Control Structures
*/

#include "loops.h"

#define whilst(what) while((what))
#define when(what) if((what))
#define but and    /* for use as in "if(a is b) but not(b is c) " */
#define nor(n) and not(n) /* if(a is b) but not(b is c) nor (c is d) */
#define otherwise else
#define commence {
#define then {
#define cease }
#define choose(what) switch((what))
#define option case
#define shatter break
#define replay continue
#define equals ==
#define is ==
#define be =
#define over >
#define above >
#define under <
#define below <
#define let /**/
#define raise(what) (what)++
#define tribute return    /* Maybe make this something else */

#define number int
#define number int
#define large long
#define small short
#define phrase(s) String(s)    /*Am I doing this part right? I confuse some C++ stuff with Java sometimes*/
#define symbol char
