#pragma once

//
// Reading temperature and relative humidity from DHT11
//

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	int8_t temp;
	int8_t rh;
} dht11_result_t;

/** Read tehmperature and humidity from the DHT11, returns false on failure */
bool dht11_read(const uint8_t pin, dht11_result_t* result);
