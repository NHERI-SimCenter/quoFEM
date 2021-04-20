#-------------------------------------------------
#
# Project created by QtCreator 2017-05-30T09:46:13
#
#-------------------------------------------------

QT       += core gui charts concurrent network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = quoFEM
TEMPLATE = app

VERSION=2.2.2
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += ../SimCenterCommon/RandomVariables
INCLUDEPATH += ../SimCenterCommon/Workflow/WORKFLOW

include($$PWD/ConanHelper.pri)

win32::LIBS+=Advapi32.lib

# linux:LIBS += /usr/lib/x86_64-linux-gnu/libcurl.so

win32 {
    RC_ICONS = icons/NHERI-quoFEM-Icon.ico
    DEFINES += USE_SIMCENTER_PYTHON
} else {
    mac {
    ICON = icons/NHERI-quoFEM-Icon.icns
    } else {
      LIBS += -lglut -lGLU -lGL
    }
}

include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/Workflow/JsonConfiguredWidgets.pri)

SOURCES += main.cpp\
        MainWindow.cpp \
        EDP.cpp  \
        FEM.cpp  \
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
    SurrogateDoEInputWidget.cpp \
    SurrogateMFInputWidget.cpp


HEADERS  += MainWindow.h \
    InputWidgetEDP.h \
    EDP.h \
    FEM.h \
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
    SurrogateDoEInputWidget.h \
    SurrogateMFInputWidget.h

FORMS    += mainwindow.ui

RESOURCES += \
    styles.qrc

# Path to build directory
win32 {
   DESTDIR = $$shell_path($$OUT_PWD)
   Release:DESTDIR = $$DESTDIR/release
   Debug:DESTDIR = $$DESTDIR/debug

   PATH_TO_BINARY=$$DESTDIR/Examples

} else {
    mac {
        PATH_TO_BINARY=$$OUT_PWD/quoFEM.app/Contents/MacOS
    }
}

win32 {

# Copies over the examples folder into the build directory
copydata.commands = $(COPY_DIR) $$shell_quote($$shell_path($$PWD/Examples)) $$shell_quote($$shell_path($$PATH_TO_BINARY))
first.depends = $(first) copydata

# Copies the dll files into the build directory
# CopyDLLs.commands = $(COPY_DIR) $$shell_quote($$shell_path($$PWD/winDLLS)) $$shell_quote($$shell_path($$DESTDIR))

first.depends += CopyDLLs

export(first.depends)
#export(CopyDLLs.commands)
export(copydata.commands)

QMAKE_EXTRA_TARGETS += first copydata CopyDLLs

}else {

  mac {
    # Copies over the examples folder into the build directory
    copydata.commands = $(COPY_DIR) \"$$shell_path($$PWD/Examples)\" \"$$shell_path($$PATH_TO_BINARY)\"
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
  }
}
