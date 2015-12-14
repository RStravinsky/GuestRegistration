#-------------------------------------------------
#
# Project created by QtCreator 2015-12-10T07:58:08
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GuestRegistration
TEMPLATE = app
RC_ICON = icon.ico
CONFIG += debug
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp

HEADERS  += mainwindow.h \
    logindialog.h

FORMS    += mainwindow.ui \
    logindialog.ui

RESOURCES += \
    resources.qrc
