#-------------------------------------------------
#
# Project created by QtCreator 2019-10-10T10:48:05
#
#-------------------------------------------------

QT             += core gui widgets

TARGET          = $$qtLibraryTarget(graph_plugin)
TEMPLATE        = lib
DESTDIR         = ../plugins
CONFIG         += plugin
INCLUDEPATH    += ../sv_tools
DEPENDPATH     +=.

EXAMPLE_FILES = graph_plugin.json

#CONFIG += c++14

SOURCES += \
    graph_plugin.cpp

HEADERS += \
    graph_plugin.h
