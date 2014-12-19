# Electronic Dice with AVR

The idea is pretty straightforward, hardware is simple, but it's still darn
cool!

The project is built around ATTiny13, but you can of course use any bigger chip
or even arduino, but it'll really be overkill. It's not making full use even of
the few peripherals the ATTiny13 provides!


## What it does?

Really simple, you have a couple LEDs that represent dots on a dice, and one
button.

When the button is down, the numbers quickly spin, and when you release
it, they slow down to a crawl, then the number blinks a few times and that's an
indication that your roll is complete.

When the dice is idle for 5 seconds, it goes to sleep (can be woken by pressing
the button). The delay can be customized in the source file.


## Hardware

The dice is not really challenging on the hardware side.

All you need is 7 LEDs with resistors, one button, and obviously the AVR.
You can also add a power switch, but it's not really needed, since the dice
is smart enough to sleep when it's not used.

It can be powered by any DC source, 3..5V. Three AA batteries in series can do
the trick, four AA's won't hurt anything as well - the chip is quite durable
if you don't overdo it.

Cunsumption when lights are on is (with my LEDs and 180R resistors) somewhat
around 50 mA, in sleep mode it's negligible (< 1 uA if my meter can be trusted).


### Pinout

```
                      +--u--+
                RST --|     |-- Vcc
    SEG_DIAG2 : PB3 --| t13 |-- PB2 : SEG_DIAG1
    SEG_HORIZ : PB4 --|     |-- PB1 : SEG_DOT
                GND --|     |-- PB0 : BUTTON
                      +-----+
```


### Placement of the LED segments

One could think that using 7 pins for 7 leds is needed, but in fact, you can use
just four - which is great, since ATTiny13 has only 5 I/O pins (unless you use
the reset pin as I/O, but that's quite uncommon).

```

    (DIAG1)         (DIAG2)

    (HORIZ)  (DOT)  (HORIZ)

    (DIAG2)         (DIAG1)

```


### Wiring of the segments

In the project, LEDs are connected by anodes to the pins. It's okay,
since the chip can handle it just fine, but you may want to connect them by
cathode instead (useful for greater loads).

To do so, adjust the `show()` routine accordingly (there's an explanatory
comment included). Basically, just negate the output states, and you're good.

```
 DOT sengment:

          PIN ---> LED -> RESISTOR ---> GND


 Other segments:

          PIN -+-> LED -> RESISTOR -+-> GND
               |                    |
               '-> LED -> RESISTOR -'

 Button:

          PIN --> BUTTON --> GND
```


## The program

Now that you have your dice hardware assembled, let's move on to the fun part -
the software.

You can of course just grab it from the repository, compile, flash and be done
with it, but I think it's useful to explain how it works.

Feel free to tinker with the source, add extra effects etc, it's fun!


### Generating random numbers

Firstly, we don't use any hardware entropy, or even a random number generator.
That may sound odd, but it's in fact not really needed.

The core idea is that the button is never pressed at the same time and for the
same duration. So, we can easily spin a counter when the button is down, and
when it's released, we pretty much already have our random number.

To add more randomness, there's also a second, "entropy" counter, that spins all
the time (unless, of course, the dice is sleeping), and when the button is
pushed, the value from this "entropy" counter is copied to the main counter.
The idea is that people can't cheat by somehow managing to press the button
for the exact same time.

Note, that those counters are just ints incrememnted and wrapped by the program
- we don't have to use of the hardware timers for this.


### Sleep mode

When the dice is inactive for five seconds (that is, no animation and
interaction), it automatically enters a POWER DOWN sleep mode.

Clearly, going to sleep without a way to wake up would be silly. Therefore, it
also enables a Pin Change Interrupt right before entering the sleep mode, so a
button press will wake it up. The cool thing is that it immediately starts
spinning the numbers and resumes normal operation.

Note, that the dice also starts into the sleep mode, so when you power it on,
it will sleep until you press the button. This is important, because I didn't
like the idea of showing the same number it starts - this way, you randomize it
right away by the first button press.
