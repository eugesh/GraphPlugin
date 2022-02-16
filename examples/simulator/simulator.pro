QT             += core gui widgets printsupport

TEMPLATE        = app
INCLUDEPATH    += ../../plugin
INCLUDEPATH    += ../../QCustomPlot

INCLUDEPATH += ../..

HEADERS += \
    ../../plugin/graph_interface.h \
    ../../plugin/graphpluginconfig.h \
    ../../plugin/common.h \
    channelconfigurationdialog.h \
    channeltuner.h \
    simulatormainwindow.h

SOURCES += \
    ../../plugin/graphpluginconfig.cpp \
    ../../plugin/common.cpp \
    channelconfigurationdialog.cpp \
    channeltuner.cpp \
    main.cpp \
    simulatormainwindow.cpp

FORMS += \
    channelconfigurationdialog.ui \
    channeltuner.ui \
    simulatormainwindow.ui

exists($$shell_path($${OUT_PWD}/configs)) {
# ToDo: test it on Win32
} else {

# Mkdirs for plugin's JSONs
windows:    mkdir.commands = $(MKDIR) $$shell_path($${OUT_PWD}/configs) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/graphs) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/config) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/si)
unix:       mkdir.commands = $(MKDIR) $$shell_path($${OUT_PWD}/configs) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/graphs) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/config) & $(MKDIR) $$shell_path($${OUT_PWD}/configs/si)

# Copy plugin's JSONs
windows:    copydata.commands = $(COPY_DIR) $$shell_path($${PWD}/../../configs) $$shell_path($${OUT_PWD}/configs)
unix:       copydata.commands = $(COPY_DIR) $$shell_path($${PWD}/../../configs) $$shell_path($${OUT_PWD}/)
}

#exists($$shell_path($${OUT_PWD}/plugins)) {
# ToDo: test it on Win32
#} else {

windows:    mkdir2.commands = $(MKDIR) $$shell_path($${OUT_PWD}/../../plugins)
unix:       mkdir2.commands = $(MKDIR) $$shell_path($${OUT_PWD}/../../plugins)

# Copy plugin's dlls
windows:    copydata2.commands = $(COPY_DIR) $$shell_path($${OUT_PWD}/../../plugins) $$shell_path($${OUT_PWD}/plugins)
unix:       copydata2.commands = $(COPY_DIR) $$shell_path($${OUT_PWD}/../../plugins) $$shell_path($${OUT_PWD}/)
#}

message(configsOUT_PWD: $${OUT_PWD}/configs)
message(pluginsOUTPWD: $${OUT_PWD}/plugins)

first.depends = $(first) mkdir copydata mkdir2 copydata2
export(first.depends)
export(mkdir.commands)
export(copydata.commands)
export(mkdir2.commands)
export(copydata2.commands)
QMAKE_EXTRA_TARGETS += first mkdir copydata mkdir2 copydata2
