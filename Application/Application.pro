TEMPLATE = app

SOURCES = \
    src/matrix.cpp \
    src/mainwindow.cpp \
    src/main.cpp \
    src/interfaces.cpp \
    src/imgview.cpp \
    src/imgmodel.cpp \
    src/imganalyzer.cpp \
    src/idataviewer.cpp \
    src/ianalysis.cpp \
    src/dataviewer.cpp \
    src/analysisplugins.cpp

HEADERS = \
    include/tiffvers.h \
    include/tiffiop.h \
    include/tiffio.h \
    include/tiffconf.h \
    include/tiff.h \
    include/matrix.h \
    include/mainwindow.h \
    include/interfaces.h \
    include/imgview.h \
    include/imgmodel.h \
    include/imganalyzer.h \
    include/idataviewer.h \
    include/ianalysis.h \
    include/dataviewer.h \
    include/analysisplugins.h

FORMS += \
    ui/progress.ui \
    ui/mw.ui \
    ui/dataViewer.ui \
    ui/data.ui

INCLUDEPATH += ./include
LIBS += -L/opt/local/lib -ltiff
RESOURCES = \
    icons.qrc

CONFIG += plugin \
 resources \
 qt \
 debug

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
