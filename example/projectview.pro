TEMPLATE = app
TARGET = projectview
INCLUDEPATH += ..

QT += gui widgets
CONFIG -= debug_and_release app_bundle
CONFIG += console

SOURCES += \
    main.cpp \
    ../projectparser.cpp \
    ../projectview.cpp \

HEADERS += \
    ../projectparser.h \
    ../projectview.h \

FORMS += \
    ../projectview.ui \

RESOURCES += \
    ../icons/projectview/projectview.qrc \
