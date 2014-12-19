Keyboard Lamp
=============

This is a keyboard lamp with adjustable brightness, that remembers last brightnbess level even when powered off.

I've made it as a replacement for my broken ThinkLight.

It uses ATtiny13, and the schematic is something like this:

                               Vcc
                                |
                  +-------------+
                  |             |
                  |  +---u---+  |
                  +--|       |--+    Buttons
                     |       |
     +--[180R]-|<|---|       |--BRT----o/ o---+
     |        LED2   |       |                |
     |             --|  t13  |--DIM----o/ o---+--- GND
    GND              |       | 
                  +--|       |----+
                  |  +-------+    |            LED1
                  |               |          +-|<|--[47R]-- Vcc
                 GND              |          |
                                   +-[10k]-|<  Transistor NPN
                                             |
                                            GND

LED1 is the bright white LED, LED2 is an indicator that the brightness has been saved. You can leave LED2 out if you want.

**Fuses** and other settings can be found in the Makefile.