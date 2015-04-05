TEMPLATE = lib
CONFIG += staticlib
TARGET = projectview
INCLUDEPATH += .

QT += gui widgets

SOURCES += \
    parser.cpp \
    projectview.cpp \

HEADERS += \
    parser.h \
    projectview.h \

FORMS += \
    projectview.ui \

RESOURCES += \
    icons/projectview/projectview.qrc \
