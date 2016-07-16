include(../common.pri)

TEMPLATE = lib
TARGET = projectview
DESTDIR = ../lib/

UI_DIR = .ui/
MOC_DIR = .moc/
OBJECTS_DIR = .obj/
RCC_DIR = .qrc/

CONFIG += staticlib
CONFIG += debug

SOURCES += \
    projectparser.cpp \
    projectview.cpp \

HEADERS += \
    projectparser.h \
    projectview.h \

FORMS += \
    projectview.ui \

include(../icons/include.pri)
