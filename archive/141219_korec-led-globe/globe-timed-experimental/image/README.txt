How to generate image header file
=================================

(intended for Linux - dunno how it works in Windoze)


1. Draw image in Gimp, with height a multiple of 8 (8, 16, 32... - how many
   LEDs you have)

2. Set collor mode to indexed (black & white) - and make sure WHITE is where
   you want your leds to light up, black where you want dark.

3. Export as C header file (eg. snowflake.h)


Now use the getbytes.py script to transform it:

    $ python3 getbytes.py snowflake.h

    #define ROWS 2
    #define COLS 15

    const uint8_t image[COLS][ROWS] PROGMEM = {
        { 0b00000001, 0b11000000 }, //       ███
        { 0b00010000, 0b10000100 }, //   █    █    █
        { 0b00111000, 0b10001110 }, //  ███   █   ███
        { 0b00011101, 0b11001100 }, //   ███ ███  ██
        { 0b00001100, 0b10011000 }, //    ██  █  ██
        { 0b00000010, 0b10100000 }, //      █ █ █
        { 0b01001001, 0b11001001 }, // █  █  ███  █  █
        { 0b11111111, 0b11111111 }, //████████████████
        { 0b01001001, 0b11001001 }, // █  █  ███  █  █
        { 0b00000010, 0b10100000 }, //      █ █ █
        { 0b00001100, 0b10010000 }, //    ██  █  █
        { 0b00011001, 0b11011100 }, //   ██  ███ ███
        { 0b00111000, 0b10001110 }, //  ███   █   ███
        { 0b00010000, 0b10000100 }, //   █    █    █
        { 0b00000001, 0b11000000 }, //       ███
    };

That will show you the transformed header file.

To store it:

    $ python3 getbytes.py snowflake.h > ../image_snowflake.h

Now, in your main.c file, just include it (remove the old image import).
