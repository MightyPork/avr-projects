all: client

client: main.c
	gcc -std=gnu99 main.c serial.c -o client

run: client
	./client /dev/ttyUSB0 9600
