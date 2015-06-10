###################################
# Makefile for MightyPork/avr-lib #
# Revision 3                      #
###################################

## ===== CPU settings =====

# CPU type
MCU	= atmega328p

# CPU frequency
F_CPU = 16000000

# Fuses
LFUSE = 0xFF
HFUSE = 0xDE
EFUSE = 0x05

OPTIMIZE = s


## ===== Source files =====

# Main C file
MAIN = main.c

# Library directory (with C and H files)
LIB_DIR = lib/

# C files in the library directory
LIB_C_FILES = uart.c iopins.c stream.c adc.c dht11.c sonar.c onewire.c spi.c sd.c sd_blockdev.c fat16.c sd_fat.c

# Extra Files that need config file:

#LIB_C_FILES += lcd.c
#LIB_C_FILES += color.c wsrgb.c
#LIB_C_FILES += debouce.c

LIB_H_FILES =  adc.h calc.h dht11.h fat16.h fat16_internal.h iopins.h nsdelay.h onewire.h
LIB_H_FILES += sd.h sd_blockdev.h sd_fat.h blockdev.h sonar.h spi.h stream.h uart.h
LIB_H_FILES += lcd.h color.h wsrgb.h debounce.h




## ===== Programmer =====

PROGRAMMER_TYPE = arduino
PROGRAMMER_ARGS = -b 57600 -P /dev/ttyUSB0


## ===== C flags =====

CFLAGS =  -std=gnu99 -mmcu=$(MCU) -DF_CPU=$(F_CPU)UL -I. -I$(LIB_DIR)
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -finline-functions
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wno-main
CFLAGS += -Wno-comment
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wfatal-errors
CFLAGS += -Wl,--gc-sections
CFLAGS += -Wl,--relax
CFLAGS += -Wl,--relax

#CFLAGS += -lm                               ## Math
#CFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## Floating-point printf
#CFLAGS += -Wl,-u,vfprintf -lprintf_min      ## Smaller printf

CFLAGS_BUILD = $(CFLAGS) -O$(OPTIMIZE)


# ---------------------------------------------------------------------------


## Defined programs / locations
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude
UART_TERM = gtkterm -p /dev/ttyUSB0

## === File lists ===
TARGET = $(strip $(basename $(MAIN)))
SRC1 = $(TARGET).c
SRC = $(SRC1)
EXTRA_SOURCE = $(addprefix $(LIB_DIR), $(LIB_C_FILES))
LIB_H_FILES_FILES = $(addprefix $(LIB_DIR), $(LIB_H_FILES))
SRC += $(EXTRA_SOURCE)

HEADERS = $(SRC:.c=.h)
OBJ = $(SRC:.c=.o)


## === File generation ===
all: $(TARGET).hex size
pre: $(TARGET).pre

%.hex: %.elf
	$(OBJCOPY) -R .eeprom -O ihex $< $@

%.elf: $(SRC) $(LIB_H_FILES_FILES) Makefile
	$(CC) $(CFLAGS_BUILD) $(SRC) --output $@

%.pre: $(SRC1)
	$(CC) $(CFLAGS) -E $(SRC1) --output $@

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

# Show debug info
debug:
	@echo
	@echo "Source files:"   $(SRC)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo


# Disassemble the ELF
disassemble: $(TARGET).lst
dis: disassemble
lst: disassemble

# Make eeprom file
eeprom: $(TARGET).eeprom

# Show how big the resulting program is
size:  $(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf

# Clean all produced trash
clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom

# Clean all trash
purge:
	rm -f *.elf *.hex *.obj *.o *.d *.eep *.lst *.lss *.sym *.map *~


## === avrdude ===

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

flashe: $(TARGET).eeprom
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U eeprom:w:$<

shell:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nt

fser: all flash ser

ser:
	$(UART_TERM)

# === fuses ===

FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) \
	           $(PROGRAMMER_ARGS) $(FUSE_STRING)
show_fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nv

set_default_fuses:  FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m
set_default_fuses:  fuses
