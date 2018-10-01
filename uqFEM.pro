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

win32:INCLUDEPATH+=../curl-7.59.0/builds/libcurl-vc-x64-release-static-ssl-static-ipv6-sspi/include
win32:LIBS += ../curl-7.59.0/builds/libcurl-vc-x64-release-static-ssl-static-ipv6-sspi/lib/libcurl_a.lib
# win32:LIBS += C:/Qt/build_msvc_static/lib/Qt5Network.lib
win32:LIBS += ../OpenSSL/lib/libeay32.lib
win32:LIBS += ../OpenSSL/lib/ssleay32.lib
win32:LIBS += wldap32.lib Normaliz.lib

win32 {
    RC_ICONS = icons/NHERI-UQFEM-Icon.ico
} else {
    mac {
    ICON = icons/NHERI-UQFEM-Icon.icns
    }
}

include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/Common/Common.pri)
# include(./MiniZip/MiniZip.pri)

DEFINES += CURL_STATICLIB

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
    CustomizedItemModel.cpp

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
    CustomizedItemModel.h

FORMS    += mainwindow.ui

RESOURCES += \
    styles.qrc
