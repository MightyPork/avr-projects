#define _POSIX_SOURCE
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "serial.h"


/** Open a serial port */
int serial_open(const char* port, const unsigned int baud)
{
	int fd = open(port, O_RDWR | O_NONBLOCK);

	struct termios tio;
	memset(&tio, 0, sizeof(tio));

	speed_t bd = baud;
	switch(bd) {
		case 1200: bd = B1200; break;
		case 1800: bd = B1800; break;
		case 2400: bd = B2400; break;
		case 4800: bd = B4800; break;
		case 9600: bd = B9600; break;
		case 19200: bd = B19200; break;
		case 38400: bd = B38400; break;
	}

	tio.c_cflag = bd | CS8 | CLOCAL | CREAD | CRTSCTS;
	tio.c_iflag = IGNPAR;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &tio);
	return fd;
}


/** Close a serial port */
int serial_close(int fd)
{
	return close(fd);
}


/** Write a byte. Returns false on failure */
int serial_write(int fd, uint8_t b)
{
	return write(fd, &b, 1);
}


int serial_puts(int fd, const char* str)
{
	int len = strlen(str);
	return write(fd, str, len);
}


uint8_t serial_read(int fd, int timeout)
{
	uint8_t b[1];
	do {
		int n = read(fd, b, 1);
		if (n == -1) return -1;
		if (n == 0) {
			usleep(1 * 1000);  // wait 1 ms
			timeout--;
			continue;
		}
		return b[0];
	} while(timeout > 0);
	return -1;
}


int serial_flush(int fd)
{
    sleep(2);
    return tcflush(fd, TCIOFLUSH);
}
