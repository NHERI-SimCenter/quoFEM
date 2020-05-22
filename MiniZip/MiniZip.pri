
INCLUDEPATH += $$PWD

win32{
    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
}

HEADERS  += $$PWD/zip.h $$PWD/ioapi.h

SOURCES += $$PWD/zip.c $$PWD/ioapi.c $$PWD/*.cpp

linux{
    LIBS += -lz
}

macx{
    LIBS += -lz
}

