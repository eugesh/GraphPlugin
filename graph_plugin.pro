#-------------------------------------------------
#
# Project created by QtCreator 2019-10-10T10:48:05
#
#-------------------------------------------------

QT             += core gui widgets printsupport

TARGET          = $$qtLibraryTarget(graph_plugin)
TEMPLATE        = lib
DESTDIR         = ../plugins
CONFIG         += plugin
INCLUDEPATH    += ../sv_tools
INCLUDEPATH    += ../QCustomPlot
DEPENDPATH     +=.

EXAMPLE_FILES = graph_plugin.json

#CONFIG += c++14

SOURCES += \
    graph_plugin.cpp \
    graphmainwindow.cpp \
    graphplugintablemodel.cpp \
    ../QCustomPlot/qcustomplot.cpp

HEADERS += \
    graph_plugin.h \
    graphmainwindow.h \
    graphplugintablemodel.h \
    ../QCustomPlot/qcustomplot.h

FORMS += \
    graphmainwindow.ui

# install
top_builddir=$$shadowed($$PWD)
target.path = $$top_builddir/sv_tools/plugins
INSTALLS += target

CONFIG += install_ok  # Do not cargo-cult this!
