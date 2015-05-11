#pragma once

//
// Utils for Dallas OneWire bus (DS1820 etc)
//

#include <stdint.h>
#include <stdbool.h>

#define SKIP_ROM  0xCC
#define CONVERT_T 0x44
#define READ_SCRATCHPAD 0xBE

/** Perform bus reset. Returns true if any device is connected */
bool ow_reset(const uint8_t pin);

/** Send a single byte */
void ow_send(const uint8_t pin, const uint8_t byte);

/** Read a single byte */
uint8_t ow_read(const uint8_t pin);

/** Wait until the device is ready. Returns false on timeout */
bool ow_wait_ready(const uint8_t pin);

/** Read bytes into an array */
void ow_read_arr(const uint8_t pin, uint8_t* array, const uint8_t count);

/** Compute a CRC16 checksum */
uint16_t crc16( uint8_t *data, uint8_t len);

/** Compute a CRC8 checksum */
uint8_t crc8(uint8_t *addr, uint8_t len);

// --- utils for DS1820 ---

#define TEMP_ERROR -32768

/**
 * Read temperature in 0.0625°C, or TEMP_ERROR on error
 * Use this where you'd normally use READ_SCRATCHPAD
 */
int16_t ds1820_read_temp(uint8_t pin);


/**
 * Read temperature in 0.1°C, or TEMP_ERROR on error
 * Use this where you'd normally use READ_SCRATCHPAD
 */
int16_t ds1820_read_temp_c(uint8_t pin);

