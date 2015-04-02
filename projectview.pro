TEMPLATE = app
TARGET = projectview
INCLUDEPATH += .

QT += gui widgets

SOURCES += \
    main.cpp \
    parser.cpp \
    projectview.cpp \

HEADERS += \
    parser.h \
    projectview.h \

RESOURCES += \
    icons/projectviewer/icons.qrc \
