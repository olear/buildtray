#-------------------------------------------------
#
# Project created by QtCreator 2015-09-14T16:51:14
#
#-------------------------------------------------

QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = buildtray
TEMPLATE = app
SOURCES += main.cpp buildtray.cpp manager.cpp
HEADERS  += buildtray.h manager.h
FORMS    += buildtray.ui
RESOURCES += buildtray.qrc
