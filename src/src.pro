include(../common.pri)

TEMPLATE = lib
TARGET = projectview
DESTDIR = ../lib/

CONFIG += staticlib

SOURCES += \
    projectparser.cpp \
    projectview.cpp \

HEADERS += \
    projectparser.h \
    projectview.h \

FORMS += \
    projectview.ui \

include(../icons/include.pri)
