TEMPLATE = app
TARGET = projectview
INCLUDEPATH += ..

QT += gui widgets

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
