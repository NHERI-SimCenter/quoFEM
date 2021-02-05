#-------------------------------------------------
#
# Project created by QtCreator 2017-05-30T09:46:13
#
#-------------------------------------------------

QT       += core gui charts concurrent network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = quoFEM
TEMPLATE = app

VERSION=2.1.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += ../SimCenterCommon/RandomVariables
INCLUDEPATH += ../SimCenterCommon/Workflow/WORKFLOW

include($$PWD/ConanHelper.pri)

win32::LIBS+=Advapi32.lib

# linux:LIBS += /usr/lib/x86_64-linux-gnu/libcurl.so

win32 {
    RC_ICONS = icons/NHERI-quoFEM-Icon.ico
} else {
    mac {
    ICON = icons/NHERI-quoFEM-Icon.icns
    }
}

include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/Workflow/JsonConfiguredWidgets.pri)

SOURCES += main.cpp\
        MainWindow.cpp \
    EDP.cpp  \
        SidebarWidgetSelection.cpp \
        InputWidgetEDP.cpp \
        InputWidgetFEM.cpp \
        UQ_EngineSelection.cpp \
        DakotaInputSampling.cpp \
        SimCenterUQInputSampling.cpp \
        DakotaInputSensitivity.cpp \
        SimCenterUQInputSensitivity.cpp \
        SimCenterUQInputSurrogate.cpp \
        UQ_Results.cpp \
        DakotaResultsSampling.cpp \
        SimCenterUQResultsSampling.cpp \
        DakotaResultsSensitivity.cpp \
        SimCenterUQResultsSensitivity.cpp \
        SimCenterUQResultsSurrogate.cpp \
        DakotaResultsCalibration.cpp \
        DakotaInputCalibration.cpp \
        UQ_Engine.cpp \
        DakotaEngine.cpp \
        SimCenterUQEngine.cpp \
        UQpyEngine.cpp \
	filterEngine.cpp \
        InputWidgetParameters.cpp \
        DakotaResultsBayesianCalibration.cpp \
        DakotaInputBayesianCalibration.cpp \
        OpenSeesParser.cpp \
        FEAPpvParser.cpp \
        OpenSeesPyParser.cpp \
    RemoteJobCreator.cpp \
    MyTableWidget.cpp \
    RemoteJobManager.cpp \
    AgaveCurl.cpp \
    RemoteService.cpp \
    CustomizedItemModel.cpp \
    UQ_MethodInputWidget.cpp \
    MonteCarloInputWidget.cpp \
    MultiFidelityMonteCarlo.cpp \
    LatinHypercubeInputWidget.cpp \
    ImportanceSamplingInputWidget.cpp \
    GaussianProcessInputWidget.cpp \
    PCEInputWidget.cpp \
    DakotaInputReliability.cpp \
    LocalReliabilityWidget.cpp \
    GlobalReliabilityWidget.cpp \
    SimCenterGraphPlot.cpp \
    qcustomplot.cpp \
    DakotaResultsReliability.cpp \
    UQ_JsonEngine.cpp \
    UCSD_Engine.cpp \
    UCSD_TMMC.cpp \
    UCSD_Results.cpp \
    CustomUQ_Results.cpp \
    SurrogateNoDoEInputWidget.cpp \
    SurrogateDoEInputWidget.cpp


HEADERS  += MainWindow.h \
    InputWidgetEDP.h \
    EDP.h \
    InputWidgetFEM.h \
    SidebarWidgetSelection.h \
    UQ_EngineSelection.h \
    DakotaInputSampling.h \
    SimCenterUQInputSampling.h \
    DakotaInputSensitivity.h \
    SimCenterUQInputSensitivity.h \
    SimCenterUQInputSurrogate.h \
    UQ_Results.h \
    DakotaResultsSampling.h \
    SimCenterUQResultsSampling.h \
    DakotaInputCalibration.h \
    DakotaResultsCalibration.h \
    DakotaResultsSensitivity.h \
    SimCenterUQResultsSensitivity.h \
    SimCenterUQResultsSurrogate.h \
    UQ_Engine.h \
    DakotaEngine.h \
    SimCenterUQEngine.h \
    UQpyEngine.h \
    filterEngine.h \
    InputWidgetParameters.h \
    DakotaInputBayesianCalibration.h \
    DakotaResultsBayesianCalibration.h \
    OpenSeesParser.h \
    FEAPpvParser.h \
    OpenSeesPyParser.h \
    RemoteJobCreator.h \
    MyTableWidget.h \
    RemoteJobManager.h \
    AgaveCurl.h \
    RemoteService.h \
    UQ_MethodInputWidget.h \
    MonteCarloInputWidget.h \
    MultiFidelityMonteCarlo.h \
    LatinHypercubeInputWidget.h \
    CustomizedItemModel.h \
    ImportanceSamplingInputWidget.h \
    GaussianProcessInputWidget.h \
    PCEInputWidget.h \
    SimCenterGraphPlot.h \
    DakotaInputReliability.h \
    LocalReliabilityWidget.h \
    GlobalReliabilityWidget.h \
    qcustomplot.h \
    DakotaResultsReliability.h \
    UQ_JsonEngine.h \
    UCSD_Engine.h \
    UCSD_TMMC.h \
    UCSD_Results.h \
    CustomUQ_Results.h \
    SurrogateNoDoEInputWidget.h \
    SurrogateDoEInputWidget.h

FORMS    += mainwindow.ui

RESOURCES += \
    styles.qrc
