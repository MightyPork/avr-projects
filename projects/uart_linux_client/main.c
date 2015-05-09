#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "serial.h"

int main(int argc, char const *argv[])
{
	if (argc < 2) { fprintf(stderr, "Missing parameter <device>\n"); return 1; }

	int baud = 9600;
	if (argc == 3) sscanf(argv[2], "%d", &baud);

	int fd = serial_open(argv[1], baud);

	// Wait for the Arduino to start
	usleep(1500000);

	// Redirecting stdin/stdout to arduino
	while(1) {
		char buf[1];
		if (fread(buf, 1, 1, stdin) > 0) {
			serial_write(fd, buf[0]);
			fflush(stdout);
		}

		char c;
		c = serial_read(fd, 10);
		if (c > 0) {
			putchar(c);
			fflush(stdout);
		}
	}
}

