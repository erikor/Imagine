HEADERS += basictoolsplugin.h ../Imagine/interfaces.h ../Imagine/matrix.h ../doubleSlider/doubleslider.h
SOURCES += basictoolsplugin.cpp ../Imagine/matrix.cpp ../doubleSlider/doubleslider.cpp
TEMPLATE = lib
LIBS = -lm
TARGET = bin/basicAnalysis
CONFIG += plugin dll release
FORMS += progress.ui size.ui sizenp.ui
macx {
  CONFIG += x86_64
}

unix:!macx{
}

win32 {
    INCLUDEPATH += c:/usr/include
    LIBS +=  -lshell32
    CONFIG += windows
}
