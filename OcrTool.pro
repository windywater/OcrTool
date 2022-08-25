TEMPLATE = app
TARGET = OcrTool
DESTDIR = $${PWD}/build
QT += core network widgets gui

OBJECTS_DIR = $${PWD}/build/obj
MOC_DIR = $${PWD}/build/moc
RCC_DIR = $${PWD}/build/rcc
UI_DIR = $${PWD}/build/ui

INCLUDEPATH += ./qxtShortCut

HEADERS += MainWindow.h \
    OverlappedWidget.h \
    ShortcutEdit.h \
    YoudaoOcr.h \

SOURCES += main.cpp \
    MainWindow.cpp \
    OverlappedWidget.cpp \
    ShortcutEdit.cpp \
    YoudaoOcr.cpp \
    
win32 {
    HEADERS += qxtShortCut/qxtglobalshortcut.h \
        qxtShortCut/qxtglobalshortcut_p.h
        
    SOURCES += qxtShortCut/qxtglobalshortcut.cpp \
        qxtShortCut/qxtglobalshortcut_win.cpp

    LIBS += -luser32
}

FORMS += MainWindow.ui
TRANSLATIONS += ocrtool_zh.ts
RESOURCES += MainWindow.qrc
