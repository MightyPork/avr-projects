PRG			= main

MCU_TARGET	= attiny13
OPTIMIZE	= 2

HZ			= 9600000UL


LFUSE		= 0x7A
HFUSE		= 0xFF




DEFS		=-std=gnu99 -funsigned-char -funsigned-bitfields -ffunction-sections -fpack-struct -fshort-enums -ffreestanding -fwhole-program -fno-inline-small-functions -fno-split-wide-types -fno-tree-scev-cprop -Wl,--relax,--gc-sections
CC			= avr-gcc
OBJCOPY		= avr-objcopy
OBJDUMP		= avr-objdump

OBJ		= $(PRG).o

override CFLAGS		= -g2 -Wall -O$(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -DF_CPU=$(HZ)

# program: $(PRG).elf lst hex
#

$(PRG).elf: $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
		avr-size -C -d --mcu=$(MCU_TARGET) $(PRG).elf

lst: $(PRG).lst

%.lst: %.elf
		$(OBJDUMP) -h -S $< > $@

hex: $(PRG).hex lst

build: hex ehex

%.hex: %.elf
		$(OBJCOPY) -j .text -j .data -O ihex $< $@


wflash: hex
		sudo avrdude -P usb -c dragon_isp -p $(MCU_TARGET) -U flash:w:$(PRG).hex


wfuses:
		sudo avrdude -P usb -c dragon_isp -p $(MCU_TARGET) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m


weeprom: ehex
		sudo avrdude -P usb -c dragon_isp -p $(MCU_TARGET) -U lfuse:w:$(LFUSE):m -U eeprom:w:$(PRG)_eeprom.hex

ee: ehex
eeprom: ehex

ehex:  $(PRG)_eeprom.hex

%_eeprom.hex: %.elf
		$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@


clean:
		rm -f *.o $(PRG).elf *.hex *.lst *~

term:
		sudo avrdude -P usb -c dragon_isp -p $(MCU_TARGET) -t

