#-------------------------------------------------
#
# Project created by QtCreator 2019-10-10T10:48:05
#
#-------------------------------------------------

QT             += core gui widgets printsupport

TARGET          = $$qtLibraryTarget(graph_plugin)
TEMPLATE        = lib
DESTDIR         = ../sv_tools/plugins
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

exists($$shell_path($${OUT_PWD}/../sv_tools/configs)) {
# ToDo: test it on Win32
} else {
    copydata.commands = $(MKDIR) $$shell_path($${OUT_PWD}/../sv_tools/configs) & $(COPY_DIR) $$shell_path($${PWD}/configs) $$shell_path($${OUT_PWD}/../sv_tools/)
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
}
