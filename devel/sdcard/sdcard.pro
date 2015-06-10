TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/avr/include

SOURCES += main.c \
    lib/adc.c \
    lib/color.c \
    lib/debounce.c \
    lib/dht11.c \
    lib/iopins.c \
    lib/lcd.c \
    lib/onewire.c \
    lib/sonar.c \
    lib/stream.c \
    lib/uart.c \
    lib/wsrgb.c \
    sd/main.c \
    sd/sd.c \
    sd/spi.c \
    lib/fat16.c \
    lib/sd.c \
    lib/spi.c \
    lib/sd_blockdev.c \
    lib/sd_fat.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    lib/adc.h \
    lib/calc.h \
    lib/color.h \
    lib/debounce.h \
    lib/dht11.h \
    lib/iopins.h \
    lib/lcd.h \
    lib/nsdelay.h \
    lib/onewire.h \
    lib/sonar.h \
    lib/stream.h \
    lib/uart.h \
    lib/wsrgb.h \
    sd/main.h \
    sd/sd.h \
    sd/sdcomm_spi.h \
    sd/spi.h \
    lib/fat16.h \
    lib/fat16_internal.h \
    lib/spi.h \
    lib/sd.h \
    lib/blockdev.h \
    lib/sd_blockdev.h \
    lib/sd_fat.h

DISTFILES += \
    Makefile \
    style.astylerc

