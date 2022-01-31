QT             += core gui widgets printsupport

TEMPLATE        = app
#DESTDIR         = ../sv_tools/plugins
#CONFIG         += plugin
INCLUDEPATH    += ../../plugin
INCLUDEPATH    += ../../QCustomPlot
#DEPENDPATH     +=.

#CONFIG += c++14

INCLUDEPATH += ../..

HEADERS += \
    ../../graph_interface.h \
    channelconfigurationdialog.h \
    channeltuner.h \
    simulatormainwindow.h

SOURCES += \
    channelconfigurationdialog.cpp \
    channeltuner.cpp \
    main.cpp \
    simulatormainwindow.cpp

#FORMS += \
#    addonplotdialog.ui

FORMS += \
    channelconfigurationdialog.ui \
    channeltuner.ui \
    simulatormainwindow.ui

# Mkdirs for plugin's JSONs
windows:    mkdir.commands = $(MKDIR) $$shell_path($${OUT_PWD}/configs) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/graphs) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/config) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/si)
unix:       mkdir.commands = $(MKDIR) $$shell_path($${OUT_PWD}/configs) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/graphs) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/config) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/si)

# Copy plugin's JSONs
windows:    copydata.commands = $(COPY_DIR) $$shell_path($${PWD}/../../configs) $$shell_path($${OUT_PWD}/configs)
unix:       copydata.commands = $(COPY_DIR) $$shell_path($${PWD}/../../configs) $$shell_path($${OUT_PWD}/)

windows:    mkdir2.commands = $(MKDIR) $$shell_path($${OUT_PWD}/plugins)
unix:       mkdir2.commands = $(MKDIR) $$shell_path($${OUT_PWD}/plugins)

# Copy plugin's dlls
windows:    copydata2.commands = $(COPY_DIR) $$shell_path($${OUT_PWD}/../../plugins) $$shell_path($${OUT_PWD}/plugins)
unix:       copydata2.commands = $(COPY_DIR) $$shell_path($${OUT_PWD}/../../plugins) $$shell_path($${OUT_PWD}/)

message(pluginsPWD: $${OUT_PWD}/../plugins)
message(pluginsOUTPWD: $${OUT_PWD}/plugins)

first.depends = $(first) mkdir copydata mkdir2 copydata2
export(first.depends)
export(mkdir.commands)
export(copydata.commands)
export(mkdir2.commands)
export(copydata2.commands)
QMAKE_EXTRA_TARGETS += first mkdir copydata mkdir2 copydata2
