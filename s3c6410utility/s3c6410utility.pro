#-------------------------------------------------
#
# Project created by QtCreator 2017-02-14T16:44:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = s3c6410utility
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logoutput.cpp \
    qsimplethread.cpp \
    diskmanager.cpp

HEADERS  += mainwindow.h \
    logoutput.h \
    qsimplethread.h \
    diskmanager.h

FORMS    += mainwindow.ui
