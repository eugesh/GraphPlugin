QT             += core gui widgets printsupport

TARGET          = $$qtLibraryTarget(graph_plugin)
TEMPLATE        = lib
DESTDIR         = ../plugins
CONFIG         += plugin
INCLUDEPATH    += ../QCustomPlot
DEPENDPATH     +=.

GRAPH_PLUGIN_VERSION_MAJOR = 1
GRAPH_PLUGIN_VERSION_MINOR = 0
GRAPH_PLUGIN_VERSION_PATCH = 0

VERSION = $$GRAPH_PLUGIN_VERSION_MAJOR.$$GRAPH_PLUGIN_VERSION_MINOR.$$GRAPH_PLUGIN_VERSION_PATCH

DEFINES += "GRAPH_PLUGIN_VERSION_MAJOR=$$GRAPH_PLUGIN_VERSION_MAJOR"
DEFINES += "GRAPH_PLUGIN_VERSION_MINOR=$$GRAPH_PLUGIN_VERSION_MINOR"
DEFINES += "GRAPH_PLUGIN_VERSION_PATCH=$$GRAPH_PLUGIN_VERSION_PATCH"

EXAMPLE_FILES = graph_plugin.json

#CONFIG += c++14

SOURCES += \
    addonplotdialog.cpp \
    common.cpp \
    digitalboarditem.cpp \
    digitaldisplayboard.cpp \
    graph_plugin.cpp \
    graphmainwindow.cpp \
    graphpluginconfig.cpp \
    graphplugintablemodel.cpp \
    ../QCustomPlot/qcustomplot.cpp \
    graphtableview.cpp \
    graphtoolbar.cpp

HEADERS += \
    addonplotdialog.h \
    common.h \
    digitalboarditem.h \
    digitaldisplayboard.h \
    graph_plugin.h \
    graphmainwindow.h \
    graphpluginconfig.h \
    graphplugintablemodel.h \
    ../QCustomPlot/qcustomplot.h \
    graphtableview.h \
    graphtoolbar.h

FORMS += \
    addonplotdialog.ui \
    digitalboarditem.ui \
    digitaldisplayboard.ui \
    graphmainwindow.ui

# install
target.path = ../plugins
INSTALLS += target
