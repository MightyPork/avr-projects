#pragma once

#include <stdint.h>


/** Open a serial port */
int serial_open(const char* port, const unsigned int baud);


/** Close a serial port */
int serial_close(int fd);


/** Write a byte. Returns false on failure */
int serial_write(int fd, uint8_t b);


/** Send a string */
int serial_puts(int fd, const char* str);


/** Read a byte */
uint8_t serial_read(int fd, int timeout);


/** Flush */
int serial_flush(int fd);
