#-------------------------------------------------
#
# Project created by QtCreator 2017-05-30T09:46:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DakotaFEM
TEMPLATE = app

INCLUDEPATH += ../widgets/RandomVariables
include(../widgets/RandomVariables/RandomVariables.pri)
include(../widgets/Common/Common.pri)

SOURCES += main.cpp\
        MainWindow.cpp \
        SidebarWidgetSelection.cpp \
    EDP.cpp \
    InputWidgetEDP.cpp \
    InputWidgetFEM.cpp \
    InputWidgetUQ.cpp \
    InputWidgetSampling.cpp

HEADERS  += MainWindow.h \
    EDP.h \
    InputWidgetEDP.h \
    InputWidgetFEM.h \
        SidebarWidgetSelection.h \
    InputWidgetUQ.h \
    InputWidgetSampling.h

FORMS    += mainwindow.ui
