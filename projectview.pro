TEMPLATE = lib
CONFIG += staticlib
TARGET = projectview
INCLUDEPATH += .

QT += gui widgets

SOURCES += \
    projectparser.cpp \
    projectview.cpp \

HEADERS += \
    projectparser.h \
    projectview.h \

FORMS += \
    projectview.ui \

RESOURCES += \
    icons/projectview/projectview.qrc \
