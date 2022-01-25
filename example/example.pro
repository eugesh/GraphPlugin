QT             += core gui widgets printsupport

TEMPLATE        = app
#DESTDIR         = ../sv_tools/plugins
#CONFIG         += plugin
INCLUDEPATH    += ..
INCLUDEPATH    += ../../QCustomPlot
#DEPENDPATH     +=.

#CONFIG += c++14

HEADERS += \
    channeltuner.h \
    simulatormainwindow.h

SOURCES += \
    channeltuner.cpp \
    main.cpp \
    simulatormainwindow.cpp

#FORMS += \
#    addonplotdialog.ui

FORMS += \
    channeltuner.ui \
    simulatormainwindow.ui
