QT += gui widgets

TEMPLATE = app
TARGET = projectview

CONFIG -= debug_and_release app_bundle

LIBS += -L../lib/ -lprojectview
INCLUDEPATH += ../include/

win32-msvc* {
	PRE_TARGETDEPS += ../lib/projectivew.lib
} else {
	PRE_TARGETDEPS += ../lib/libprojectview.a
}

SOURCES += main.cpp

RESOURCES += \
    ../icons/projectview/projectview.qrc \
