#-------------------------------------------------
#
# Project created by QtCreator 2016-03-10T15:16:08
#
#-------------------------------------------------

QT       += widgets

TARGET = ScopeViewPlugin
TEMPLATE = lib

DEFINES += TPLUGIN_LIBRARY

SOURCES += scopeviewplugin.cpp \
    scopeviewform.cpp \
    ../Common/uutbutton.cpp

HEADERS += scopeviewplugin.h \
    scopeviewform.h \
    ../Common/uutbutton.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS +=
