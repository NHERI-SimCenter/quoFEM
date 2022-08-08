include(quoFEM.pro)

SOURCES  -= main.cpp
SOURCES  += "Tests/main.cpp"

#HEADERS  += "Tests/MainCommandLineWorkflowApp2.h"

CONFIG   += cmdline
QT       -= gui

TARGET   = qfem
