TEMPLATE = lib
LIBS = -lm
TARGET = ./plugins/basicAnalysis
CONFIG += plugin dll release

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

HEADERS += \
    include/doubleslider.h \
    include/basictoolsplugin.h

SOURCES += \
    src/basictoolsplugin.cpp

FORMS += \
    ui/sizenp.ui \
    ui/size.ui \
    ui/progress.ui
