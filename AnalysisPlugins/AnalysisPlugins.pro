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

HEADERS += \
    include/doubleslider.h \
    include/basictoolsplugin.h \
    include/interfaces.h \
    include/analysisplugins.h \
    include/matrix.h

SOURCES += \
    src/basictoolsplugin.cpp \
    src/matrix.cpp \
    src/doubleslider.cpp

FORMS += \
    ui/sizenp.ui \
    ui/size.ui \
    ui/progress.ui
