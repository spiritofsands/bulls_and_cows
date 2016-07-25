#-------------------------------------------------
#
# Project created by QtCreator 2016-07-25T03:28:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -Wfatal-errors -pedantic-errors

CONFIG += c++11


TARGET = bulls_and_cows
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bullsandcowssolver.cpp

HEADERS  += mainwindow.h \
    bullsandcowssolver.h

FORMS    += mainwindow.ui
