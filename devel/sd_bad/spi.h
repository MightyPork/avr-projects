#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void spi_initialize();
void spi_set_divisor(unsigned int divisor);
void spi_cs_assert();
void spi_cs_deassert();
void spi_send_byte(uint8_t input);
uint8_t spi_rcv_byte();
void spi_enable();
void spi_disable();

#endif
