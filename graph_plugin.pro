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
    addonplotdialog.cpp \
    graph_plugin.cpp \
    graphmainwindow.cpp \
    graphpluginconfig.cpp \
    graphplugintablemodel.cpp \
    ../QCustomPlot/qcustomplot.cpp \
    graphtableview.cpp

HEADERS += \
    addonplotdialog.h \
    common.h \
    graph_plugin.h \
    graphmainwindow.h \
    graphpluginconfig.h \
    graphplugintablemodel.h \
    ../QCustomPlot/qcustomplot.h \
    graphtableview.h

FORMS += \
    addonplotdialog.ui \
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
