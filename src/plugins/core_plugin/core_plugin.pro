#-------------------------------------------------
#
# Project created by QtCreator 2016-06-26T17:31:13
#
#-------------------------------------------------

DEFINES += CORE_PLUGIN_LIBRARY

include(../../../nwpc-monitor-app.pri)

QT       += core gui widgets

TARGET = core_plugin
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH+=../../libs

LIBS += -L$$build_lib_dir -lplugin_system

DESTDIR = $$build_base_dir/plugins

SOURCES += core_plugin.cpp \
    mainwindow.cpp \
    iperspective.cpp \
    welcome_perspective.cpp \
    welcome_widget.cpp

HEADERS += core_plugin.h \
    core_plugin_global.h \
    mainwindow.h \
    iperspective.h \
    welcome_perspective.h \
    welcome_widget.h

DISTFILES += core_plugin.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    mainwindow.ui \
    welcome_widget.ui
