#-------------------------------------------------
#
# Project created by QtCreator 2017-05-30T09:46:13
#
#-------------------------------------------------

QT       += core gui charts concurrent network printsupport 3dcore 3drender 3dextras webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = quoFEM
TEMPLATE = app

VERSION=2.3.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += _GRAPHICS_Qt3D

INCLUDEPATH += ../SimCenterCommon/RandomVariables
INCLUDEPATH += ../SimCenterCommon/Workflow/WORKFLOW
INCLUDEPATH += FEM
INCLUDEPATH += EDP

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

include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/RandomVariables/RandomVariables.pri)
#include(../SimCenterCommon/Workflow/JsonConfiguredWidgets.pri)
include(../SimCenterCommon/Workflow/Workflow.pri)
include(../SimCenterCommon/InputSheetBM/InputSheetBM.pri)


SOURCES += main.cpp\
    WorkflowApp_quoFEM.cpp \
    RunWidget.cpp \
    EDP/quoEDP.cpp  \
    EDP/InputWidgetEDP.cpp \
    FEM/FEM_Selection.cpp \
    FEM/OpenSeesFEM.cpp \
    FEM/OpenSeesPyFEM.cpp \
    FEM/FEAPpvFEM.cpp \    
    FEM/FEAPpvParser.cpp \
    FEM/OpenSeesPyParser.cpp \
    FEM/surrogateGpFEM.cpp \
    FEM/surrogateGpParser.cpp \
    FEM/CustomFEM.cpp \

HEADERS  += WorkflowApp_quoFEM.h \    
    RunWidget.h \        
    EDP/InputWidgetEDP.h \
    EDP/quoEDP.h \
    FEM/FEM_Selection.h \
    FEM/OpenSeesFEM.h \
    FEM/OpenSeesPyFEM.h \
    FEM/FEAPpvFEM.h \    
    FEM/FEAPpvParser.h \
    FEM/OpenSeesPyParser.h \
    FEM/surrogateGpFEM.h \
    FEM/surrogateGpParser.h \
    FEM/CustomFEM.h \

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

}

