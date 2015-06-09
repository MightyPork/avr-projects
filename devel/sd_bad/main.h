#ifndef MAIN_H
#define MAIN_H

#include "sdcomm_spi.h"

/* Peripheral clock rate, in Hz, used for timing. */
#define PERIPH_CLOCKRATE 8000000
int do_sd_initialize(sd_context_t *sdc);

#endif
