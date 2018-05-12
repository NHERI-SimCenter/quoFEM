#-------------------------------------------------
#
# Project created by QtCreator 2017-05-30T09:46:13
#
#-------------------------------------------------

QT       += core gui charts concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = uqFEM
TEMPLATE = app

INCLUDEPATH += ../widgets/RandomVariables
INCLUDEPATH += ../simcenterAgave/interface

macos:LIBS += /usr/lib/libcurl.dylib
win32:INCLUDEPATH+=../libCurl-7.59.0/include
win32:LIBS += ../libCurl-7.59.0/lib/libcurl.lib

include(../widgets/RandomVariables/RandomVariables.pri)
include(../widgets/Common/Common.pri)

SOURCES += main.cpp\
        MainWindow.cpp \
        SidebarWidgetSelection.cpp \
        EDP.cpp \
        InputWidgetEDP.cpp \
        InputWidgetFEM.cpp \
        InputWidgetUQ.cpp \
        InputWidgetSampling.cpp \
        DakotaResults.cpp \
        DakotaResultsSampling.cpp \
        DakotaResultsCalibration.cpp \
        InputWidgetCalibration.cpp \
        InputWidgetDakotaMethod.cpp \
        InputWidgetParameters.cpp \
        DakotaResultsBayesianCalibration.cpp \
        InputWidgetBayesianCalibration.cpp \
        OpenSeesParser.cpp \
        FEAPpvParser.cpp \
        ../simcenterAgave/interface/AgaveCLI.cpp \
    RemoteJobCreatorWidget.cpp \
    RemoteJobManagerWidget.cpp \
    MyTableWidget.cpp \
    ../simcenterAgave/interface/AgaveInterface.cpp \
    ../simcenterAgave/interface/AgaveCurl.cpp

HEADERS  += MainWindow.h \
    EDP.h \
    InputWidgetEDP.h \
    InputWidgetFEM.h \
    SidebarWidgetSelection.h \
    InputWidgetUQ.h \
    InputWidgetSampling.h \
    DakotaResults.h \
    DakotaResultsSampling.h \
    InputWidgetCalibration.h \
    DakotaResultsCalibration.h \
    InputWidgetDakotaMethod.h \
    InputWidgetParameters.h \
    InputWidgetBayesianCalibration.h \
    InputWidgetBayesianCalibration.h \
    DakotaResultsBayesianCalibration.h \
    OpenSeesParser.h \
    FEAPpvParser.h \
    ../simcenterAgave/interface/AgaveCLI.h \
    RemoteJobCreatorWidget.h \
    RemoteJobManagerWidget.h \
    MyTableWidget.h \
    ../simcenterAgave/interface/AgaveInterface.h \
    ../simcenterAgave/interface/AgaveCurl.h

FORMS    += mainwindow.ui
