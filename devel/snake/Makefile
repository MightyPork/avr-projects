
MCU	= atmega328p

F_CPU = 16000000

LFUSE = 0xFF
HFUSE = 0xDE
EFUSE = 0x05

MAIN = main.c

## If you've split your program into multiple files,
## include the additional .c source (in same directory) here
## (and include the .h files in your foo.c)
LOCAL_SOURCE =

## Here you can link to one more directory (and multiple .c files)
# EXTRA_SOURCE_DIR = ../AVR-Programming-Library/
EXTRA_SOURCE_DIR =
EXTRA_SOURCE_FILES =



##########------------------------------------------------------##########
##########                 Programmer Defaults                  ##########
##########          Set up once, then forget about it           ##########
##########        (Can override.  See bottom of file.)          ##########
##########------------------------------------------------------##########
#19200
PROGRAMMER_TYPE = arduino
PROGRAMMER_ARGS = -b 57600 -P /dev/ttyUSB0


##########------------------------------------------------------##########
##########                   Makefile Magic!                    ##########
##########         Summary:                                     ##########
##########             We want a .hex file                      ##########
##########        Compile source files into .elf                ##########
##########        Convert .elf file into .hex                   ##########
##########        You shouldn't need to edit below.             ##########
##########------------------------------------------------------##########

## Defined programs / locations
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = sudo avrdude

## Compilation options, type man avr-gcc if you're curious.
CFLAGS = -std=gnu99 -mmcu=$(MCU) -DF_CPU=$(F_CPU)UL -I. -I$(EXTRA_SOURCE_DIR)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wno-main -Wno-strict-prototypes -Wno-comment
CFLAGS += -g2 -Wextra -Wfatal-errors
CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--relax

CFLAGS_BUILD = $(CFLAGS) -Os

# CFLAGS += -lm
## CFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
## CFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf

## Lump target and extra source files together
TARGET = $(strip $(basename $(MAIN)))
SRC1 = $(TARGET).c
SRC = $(SRC1)
EXTRA_SOURCE = $(addprefix $(EXTRA_SOURCE_DIR), $(EXTRA_SOURCE_FILES))
SRC += $(EXTRA_SOURCE)
SRC += $(LOCAL_SOURCE)

## List of all header files
HEADERS = $(SRC:.c=.h)

## For every .c file, compile an .o object file
OBJ = $(SRC:.c=.o)

## Generic Makefile targets.  (Only .hex file is necessary)
all: $(TARGET).hex size
pre: $(TARGET).pre

%.hex: %.elf
	$(OBJCOPY) -R .eeprom -O ihex $< $@

%.elf: $(SRC)
	$(CC) $(CFLAGS_BUILD) $(SRC) --output $@

%.pre: $(SRC1)
	$(CC) $(CFLAGS) -E $(SRC1) --output $@

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

debug:
	@echo
	@echo "Source files:"   $(SRC)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo

# Optionally create listing file from .elf
# This creates approximate assembly-language equivalent of your code.
# Useful for debugging time-sensitive bits,
# or making sure the compiler does what you want.
disassemble: $(TARGET).lst

dis: disassemble
lst: disassemble

eeprom: $(TARGET).eeprom

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

# Optionally show how big the resulting program is
size:  $(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom

squeaky_clean:
	rm -f *.elf *.hex *.obj *.o *.d *.eep *.lst *.lss *.sym *.map *~


##########------------------------------------------------------##########
##########              Programmer-specific details             ##########
##########           Flashing code to AVR using avrdude         ##########
##########------------------------------------------------------##########

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

flash_eeprom: $(TARGET).eeprom
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U eeprom:w:$<

terminal:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nt


flash_arduino: PROGRAMMER_TYPE = arduino
flash_arduino: PROGRAMMER_ARGS =
flash_arduino: flash

flash_dragon_isp: PROGRAMMER_TYPE = dragon_isp
flash_dragon_isp: PROGRAMMER_ARGS =
flash_dragon_isp: flash


##########------------------------------------------------------##########
##########       Fuse settings and suitable defaults            ##########
##########------------------------------------------------------##########

## Generic
FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) \
	           $(PROGRAMMER_ARGS) $(FUSE_STRING)
show_fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nv

## Called with no extra definitions, sets to defaults
set_default_fuses:  FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m
set_default_fuses:  fuses
