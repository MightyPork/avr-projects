## === CPU settings ===
# CPU type
MCU	= atmega328p
# CPU frequency
F_CPU = 16000000
# Fuses
LFUSE = 0xFF
HFUSE = 0xDE
EFUSE = 0x05


## === Source files ===
# Main C file
MAIN = main.c
# Extra C files in this folder
LOCAL_SOURCE =

# Library directory (with C files)
EXTRA_SOURCE_DIR = lib/
# C files in the library directory
EXTRA_SOURCE_FILES = uart.c uart_ansi.c


## === Programmer ===
PROGRAMMER_TYPE = arduino
PROGRAMMER_ARGS = -b 57600 -P /dev/ttyUSB0


## === C flags ===

CFLAGS = -std=gnu99 -mmcu=$(MCU) -DF_CPU=$(F_CPU)UL -I. -I$(EXTRA_SOURCE_DIR)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wno-main -Wno-strict-prototypes -Wno-comment
CFLAGS += -g2 -Wextra -Wfatal-errors -Wno-unused-but-set-variable
CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--relax
# CFLAGS += -lm  ## Math
# CFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
# CFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf
CFLAGS_BUILD = $(CFLAGS) -Os


# ---------------------------------------------------------------------------

## Defined programs / locations
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

## === File lists ===
TARGET = $(strip $(basename $(MAIN)))
SRC1 = $(TARGET).c
SRC = $(SRC1)
EXTRA_SOURCE = $(addprefix $(EXTRA_SOURCE_DIR), $(EXTRA_SOURCE_FILES))
SRC += $(EXTRA_SOURCE)
SRC += $(LOCAL_SOURCE)

HEADERS = $(SRC:.c=.h)
OBJ = $(SRC:.c=.o)


## === File generation ===
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


# === fuses ===

FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) \
	           $(PROGRAMMER_ARGS) $(FUSE_STRING)
show_fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nv

set_default_fuses:  FUSE_STRING = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m
set_default_fuses:  fuses
