TEMPLATE = app
SOURCES = mainwindow.cpp \
 main.cpp \
 interfaces.cpp \
 imgmodel.cpp \
 imganalyzer.cpp \
 imgview.cpp \
 dataviewer.cpp
HEADERS = mainwindow.h \
 ianalysis.h \
 interfaces.h \
 imgmodel.h \
 imganalyzer.h \
 imgview.h \
 dataviewer.h \
    include/tiffvers.h \
    include/tiffiop.h \
    include/tiffio.h \
    include/tiffconf.h \
    include/tiff.h
FORMS = mw.ui data.ui
LIBS += -L/opt/local/lib -ltiff
RESOURCES = icons.qrc
RC_FILE = application.rc
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
