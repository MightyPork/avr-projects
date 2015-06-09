TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/avr/include

include(deployment.pri)
qtcAddDeployment()

DISTFILES += \
    Makefile \
    style.astylerc

HEADERS += \
    main.h \
    sd.h \
    sdcomm_spi.h \
    spi.h

SOURCES += \
    main.c \
    sd.c \
    spi.c

