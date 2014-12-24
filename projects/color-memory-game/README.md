Color Memory Game
=================

This is a memory game (with pairs of cards), played on a WS2812B RGB LED strip.

The code is designed for *Arduino Pro Mini* (16 MHz), and can be flashed with avrdude (use the Makefile) using a USB-to-serial adapter.

Peripherals
-----------

- WS2812B RGB LED strip (or chained pixels) for the game board
- 4 buttons for navigation (arrows)
- 2 buttons for RESTART and SELECT

The LED strip should have a large capacitor (~ 1000uF) in parallel to the power supply, to avoid flickering.

It's connected over a resistor (470R will do fine) to the Arduino.

All pins can be adjusted in the `main.c` near the top, also the board size can be changed (for more than 15 card pairs, you'll have to add more colors).

Board
-----

The LEDs are numbered in this manner:

     0  1  2  3
     4  5  6  7
     8  9 10 11
    12 13 14 15

Set the board width and height in `main.c` to match your setup.
