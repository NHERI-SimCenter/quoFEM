#-------------------------------------------------
#
# Project created by QtCreator 2017-05-30T09:46:13
#
#-------------------------------------------------

QT       += core gui charts concurrent network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = uqFEM
TEMPLATE = app

VERSION=1.1.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += ../SimCenterCommon/RandomVariables
# INCLUDEPATH += ../simcenterAgave/interface

macos:LIBS += /usr/lib/libcurl.dylib
win32:INCLUDEPATH += "C:/Adam/Qt/curl/include"
win32:LIBS += C:/Adam/Qt/curl/lib/libcurl.lib
#win32:INCLUDEPATH+=../libCurl-7.59.0/include
#win32:LIBS += ../libCurl-7.59.0/lib/libcurl.lib


win32 {
    RC_ICONS = icons/NHERI-UQFEM-Icon.ico
} else {
    mac {
    ICON = icons/NHERI-UQFEM-Icon.icns
    }
}

include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/Common/Common.pri)
include(./MiniZip/MiniZip.pri)

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
    RemoteJobCreator.cpp \
    MyTableWidget.cpp \
    RemoteJobManager.cpp \
    AgaveCurl.cpp \
    CustomizedItemModel.cpp \
    MonteCarloInputWidget.cpp \
    LatinHypercubeInputWidget.cpp


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
    RemoteJobCreator.h \
    MyTableWidget.h \
    RemoteJobManager.h \
    AgaveCurl.h \
    MonteCarloInputWidget.h \
    LatinHypercubeInputWidget.h \
    CustomizedItemModel.h

FORMS    += mainwindow.ui

RESOURCES += \
    styles.qrc
