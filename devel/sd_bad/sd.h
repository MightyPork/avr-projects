#ifndef SD_H
#define SD_H

#include "sdcomm_spi.h"
#include <stdint.h>

#define SD_BLOCKSIZE 512
#define SD_BLOCKSIZE_NBITS 9

/* User functions */
int sd_card_present();
int sd_initialize(sd_context_t *sdc);
int sd_read_block(sd_context_t *sdc, uint32_t blockaddr, uint8_t *data);
int sd_write_block(sd_context_t *sdc, uint32_t blockaddr, uint8_t *data);
void sd_wait_notbusy(sd_context_t *sdc);

/* Internal functions, used for SD card communications. */
void sd_packarg(uint8_t *argument, uint32_t value);
int sd_set_blocklen(sd_context_t *sdc, uint32_t length);
int sd_send_command(sd_context_t *sdc,
					uint8_t cmd, uint8_t response_type,
					uint8_t *response, uint8_t *argument);
void sd_delay(char number);

#endif
