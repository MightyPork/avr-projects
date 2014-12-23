PRG		= main

MCU_TARGET	= attiny2313
OPTIMIZE	= 2


LFUSE		= 0xE4
HFUSE		= 0xDF




DEFS		=-std=gnu99 -funsigned-char -funsigned-bitfields -ffunction-sections -fpack-struct -fshort-enums -ffreestanding --combine -fwhole-program -fno-inline-small-functions -fno-split-wide-types -fno-tree-scev-cprop -Wl,--relax,--gc-sections
CC			= avr-gcc
OBJCOPY		= avr-objcopy
OBJDUMP		= avr-objdump

OBJ		= $(PRG).o

override CFLAGS		= -g2 -Wall -O$(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)

# program: $(PRG).elf lst hex
#

all: $(PRG).elf lst eeprom

$(PRG).elf: $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
		avr-size -C -d --mcu=$(MCU_TARGET) $(PRG).elf

lst: $(PRG).lst

%.lst: %.elf
		$(OBJDUMP) -h -S $< > $@

hex: $(PRG).hex lst

%.hex: %.elf
		$(OBJCOPY) -j .text -j .data -O ihex $< $@


install: wflash wfuses

install_ee: weeprom
install_fl: wflash
install_fu: wfuses


wflash:  $(PRG).hex
		avrdude -P usb -c dragon_isp -p $(MCU_TARGET) -B 16 -U flash:w:$(PRG).hex


wfuses:
		avrdude -P usb -c dragon_isp -p $(MCU_TARGET) -B 16 -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m


weeprom: ehex
		avrdude -P usb -c dragon_isp -p $(MCU_TARGET) -B 16 -U lfuse:w:$(LFUSE):m -U eeprom:w:$(PRG)_eeprom.hex

ee: ehex
eeprom: ehex

ehex:  $(PRG)_eeprom.hex

%_eeprom.hex: %.elf
		$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@


clean:
		rm -f *.o $(PRG).elf *.hex *.lst *~

term:
		avrdude -P usb -c dragon_isp -p $(MCU_TARGET) -B 16 -t

