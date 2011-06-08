TEMPLATE = lib
LIBS = -lm
TARGET = ./plugins/basicAnalysis
CONFIG += plugin dll release
INCLUDEPATH += ./include

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

SOURCES += \
    src/segmentation.cpp \
    src/matrix.cpp \
    src/doubleslider.cpp

FORMS += \
    ui/manual.ui \
    ui/halo.ui

HEADERS += \
    include/segmentation.h \
    include/matrix.h \
    include/interfaces.h \
    include/doubleslider.h
