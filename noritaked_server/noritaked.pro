#-------------------------------------------------
#
# Project created by QtCreator 2015-04-19T01:08:26
#
#-------------------------------------------------

#
#	Noritake Display Daemon Server Side
#	by IDENT Software ~ http://identsoft.org
#


QT       += core serialport

QT       -= gui

TARGET = noritaked
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    noritakedaemon.cpp \
    qaimpinfo.cpp

HEADERS += \
    noritakedaemon.h \
    qaimpinfo.h
