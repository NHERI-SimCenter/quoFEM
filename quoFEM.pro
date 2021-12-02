#-------------------------------------------------
#
# Project created by QtCreator 2017-05-30T09:46:13
#
#-------------------------------------------------

QT       += core gui charts concurrent network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = quoFEM
TEMPLATE = app

VERSION=2.3.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += ../SimCenterCommon/RandomVariables
INCLUDEPATH += ../SimCenterCommon/Workflow/WORKFLOW
INCLUDEPATH += UQ
INCLUDEPATH += UQ/dakota
INCLUDEPATH += UQ/simcenterUQ
INCLUDEPATH += UQ/ucsd
INCLUDEPATH += UQ/customUQ
INCLUDEPATH += UQe/common
INCLUDEPATH += FEM

include($$PWD/ConanHelper.pri)

win32::LIBS+=Advapi32.lib

# linux:LIBS += /usr/lib/x86_64-linux-gnu/libcurl.so

win32 {
    RC_ICONS = icons/NHERI-quoFEM-Icon.ico
    DEFINES += USE_SIMCENTER_PYTHON
} else {
    mac {
    ICON = icons/NHERI-quoFEM-Icon.icns
    QMAKE_INFO_PLIST=$$PWD/Info.plist    
    } else {
      LIBS += -lglut -lGLU -lGL
    }
}

include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/Workflow/JsonConfiguredWidgets.pri)


SOURCES += main.cpp\
    BayesPlots.cpp \
    MainWindow.cpp \
    EDP.cpp  \
    SidebarWidgetSelection.cpp \
    InputWidgetEDP.cpp \
    filterEngine.cpp \
    InputWidgetParameters.cpp \
    RemoteJobCreator.cpp \
    MyTableWidget.cpp \
    RemoteJobManager.cpp \
    AgaveCurl.cpp \
    RemoteService.cpp \
    CustomizedItemModel.cpp \
    SimCenterGraphPlot.cpp \
    qcustomplot.cpp \
    FEM/InputWidgetFEM.cpp \
    FEM/FEM.cpp  \    
    FEM/OpenSeesParser.cpp \
    FEM/FEAPpvParser.cpp \
    FEM/OpenSeesPyParser.cpp \
    UQ/UQ_EngineSelection.cpp \
    UQ/UQ_MethodInputWidget.cpp \
    UQ/UQ_Results.cpp \
    UQ/UQ_Engine.cpp \
    UQ/UQpyEngine.cpp \    
    UQ/customUQ/CustomUQ_Results.cpp \
    UQ/common/ResultsDataChart.cpp \
    UQ/UQ_JsonEngine.cpp \
    UQ/dakota/DakotaEngine.cpp \
    UQ/dakota/DakotaInputSampling.cpp \
    UQ/dakota/DakotaResultsSampling.cpp \
    UQ/dakota/MonteCarloInputWidget.cpp \
    UQ/dakota/MultiFidelityMonteCarlo.cpp \
    UQ/dakota/LatinHypercubeInputWidget.cpp \
    UQ/dakota/ImportanceSamplingInputWidget.cpp \
    UQ/dakota/GaussianProcessInputWidget.cpp \
    UQ/dakota/PCEInputWidget.cpp \
    UQ/dakota/DakotaInputBayesianCalibration.cpp \
    UQ/dakota/DakotaResultsBayesianCalibration.cpp \
    UQ/dakota/DakotaResultsCalibration.cpp \
    UQ/dakota/DakotaInputCalibration.cpp \
    UQ/dakota/DakotaInputSensitivity.cpp \
    UQ/dakota/DakotaResultsSensitivity.cpp \
    UQ/dakota/DakotaInputReliability.cpp \
    UQ/dakota/DakotaResultsReliability.cpp \    
    UQ/dakota/LocalReliabilityWidget.cpp \
    UQ/dakota/GlobalReliabilityWidget.cpp \
    UQ/ucsd/UCSD_Engine.cpp \
    UQ/ucsd/UCSD_TMMC.cpp \
    UQ/ucsd/UCSD_Results.cpp \
    UQ/simcenterUQ/SimCenterUQEngine.cpp \
    UQ/simcenterUQ/SimCenterUQInputSampling.cpp \
    UQ/simcenterUQ/SimCenterUQResultsSampling.cpp \    
    UQ/simcenterUQ/SimCenterUQInputSensitivity.cpp \
    UQ/simcenterUQ/SimCenterUQResultsSensitivity.cpp \    
    UQ/simcenterUQ/SimCenterUQInputSurrogate.cpp \
    UQ/simcenterUQ/SimCenterUQResultsSurrogate.cpp \
    UQ/simcenterUQ/SurrogateNoDoEInputWidget.cpp \
    UQ/simcenterUQ/SurrogateDoEInputWidget.cpp \
    UQ/simcenterUQ/SurrogateMFInputWidget.cpp


HEADERS  += MainWindow.h \
    BayesPlots.h \
    InputWidgetEDP.h \
    EDP.h \
    SidebarWidgetSelection.h \
    filterEngine.h \
    InputWidgetParameters.h \
    RemoteJobCreator.h \
    MyTableWidget.h \
    RemoteJobManager.h \
    AgaveCurl.h \
    RemoteService.h \
    CustomizedItemModel.h \
    SimCenterGraphPlot.h \
    qcustomplot.h \
    FEM/FEM.h \
    FEM/InputWidgetFEM.h \
    FEM/OpenSeesParser.h \
    FEM/FEAPpvParser.h \
    FEM/OpenSeesPyParser.h \
    UQ/UQ_EngineSelection.h \
    UQ/UQ_Results.h \
    UQ/UQ_Engine.h \
    UQ/UQpyEngine.h \
    UQ/UQ_MethodInputWidget.h \    
    UQ/common/ResultsDataChart.h \
    UQ/UQ_JsonEngine.h \
    UQ/dakota/DakotaEngine.h \    
    UQ/dakota/DakotaInputSampling.h \
    UQ/dakota/DakotaInputCalibration.h \
    UQ/dakota/DakotaResultsCalibration.h \
    UQ/dakota/DakotaResultsSensitivity.h \    
    UQ/dakota/DakotaResultsSampling.h \    
    UQ/dakota/DakotaInputSensitivity.h \
    UQ/dakota/DakotaInputBayesianCalibration.h \
    UQ/dakota/DakotaResultsBayesianCalibration.h \
    UQ/dakota/MonteCarloInputWidget.h \
    UQ/dakota/MultiFidelityMonteCarlo.h \
    UQ/dakota/LatinHypercubeInputWidget.h \
    UQ/dakota/ImportanceSamplingInputWidget.h \
    UQ/dakota/GaussianProcessInputWidget.h \
    UQ/dakota/PCEInputWidget.h \
    UQ/dakota/DakotaInputReliability.h \
    UQ/dakota/DakotaResultsReliability.h \    
    UQ/dakota/LocalReliabilityWidget.h \
    UQ/dakota/GlobalReliabilityWidget.h \
    UQ/ucsd/UCSD_Engine.h \
    UQ/ucsd/UCSD_TMMC.h \
    UQ/ucsd/UCSD_Results.h \
    UQ/customUQ/CustomUQ_Results.h \
    UQ/simcenterUQ/SimCenterUQEngine.h \    
    UQ/simcenterUQ/SimCenterUQInputSampling.h \    
    UQ/simcenterUQ/SimCenterUQInputSensitivity.h \
    UQ/simcenterUQ/SimCenterUQInputSurrogate.h \
    UQ/simcenterUQ/SimCenterUQResultsSensitivity.h \
    UQ/simcenterUQ/SimCenterUQResultsSurrogate.h \    
    UQ/simcenterUQ/SimCenterUQResultsSampling.h \    
    UQ/simcenterUQ/SurrogateNoDoEInputWidget.h \
    UQ/simcenterUQ/SurrogateDoEInputWidget.h \
    UQ/simcenterUQ/SurrogateMFInputWidget.h

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
