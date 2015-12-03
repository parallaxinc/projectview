INCLUDEPATH += $$PWD/include/
LIBS        += -L$$PWD/lib/ -lprojectview

win32-msvc* {
	PRE_TARGETDEPS += $$PWD/lib/projectview.lib
} else {
	PRE_TARGETDEPS += $$PWD/lib/libprojectview.a
}

include($$PWD/icons/include.pri)
