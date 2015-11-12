
TEMPLATE = lib
TARGET = projectview
DESTDIR = ../lib/

CONFIG += staticlib
INCLUDEPATH += .

QT += gui widgets
CONFIG -= debug_and_release app_bundle

SOURCES += \
    projectparser.cpp \
    projectview.cpp \

HEADERS += \
    projectparser.h \
    projectview.h \

FORMS += \
    projectview.ui \

RESOURCES += \
    ../icons/projectview/projectview.qrc \
