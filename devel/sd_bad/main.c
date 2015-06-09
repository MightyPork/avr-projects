#include "main.h"
#include "sd.h"
#include "spi.h"

static uint8_t sd_buffer[512];
static sd_context_t sdc;

void main(void)
{
	int j, ok;
	ok = 0;

	/* Stop the watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	/* Set some reasonable values for the timeouts.
	 */
	sdc.timeout_write = PERIPH_CLOCKRATE / 8;
	sdc.timeout_read = PERIPH_CLOCKRATE / 20;

	sdc.busyflag = 0;
	for (j = 0; j < SD_INIT_TRY && ok != 1; j++)
	{
		ok = do_sd_initialize(&sdc);
	}

	/* Read in the first block on the SD Card */
	if (ok == 1)
	{
		sd_read_block(&sdc, 0, sd_buffer);
		sd_wait_notbusy(&sdc);
	}

	/* Wait forever */
	while (1) { }
}


int do_sd_initialize(sd_context_t *sdc)
{
	/* Initialize the SPI controller */
	spi_initialize();

	/* Start out with a slow SPI clock, 400kHz, as required
	 by the SD spec
	(for MMC compatibility). */
	spi_set_divisor(PERIPH_CLOCKRATE / 400000);

	/* Initialization OK? */
	if (sd_initialize(sdc) != 1)
		return 0;

	/* Set the maximum SPI clock rate possible */
	spi_set_divisor(2);

	return 1;
}
