
TEMPLATE = app
TARGET = OcrTool
DESTDIR = $${PWD}/build
QT += core network widgets gui
DEFINES += QT_NETWORK_LIB QT_WIDGETS_LIB

OBJECTS_DIR = $${PWD}/build/obj
MOC_DIR = $${PWD}/build/moc
RCC_DIR = $${PWD}/build/rcc
UI_DIR = $${PWD}/build/ui

INCLUDEPATH += ./qxtShortCut

HEADERS += DragDropProxy.h \
    GlobalSettings.h \
    MainWindow.h \
    OverlappedWidget.h \
    SettingDialog.h \
    ShortcutEdit.h \
    SogouOcr.h \

SOURCES += DragDropProxy.cpp \
    GlobalSettings.cpp \
    main.cpp \
    MainWindow.cpp \
    OverlappedWidget.cpp \
    SettingDialog.cpp \
    ShortcutEdit.cpp \
    SogouOcr.cpp \
    
win:{
    HEADERS += qxtShortCut/qxtglobalshortcut.h \
        qxtShortCut/qxtglobalshortcut_p.h
        
    SOURCES += qxtShortCut/qxtglobalshortcut.cpp \
        qxtShortCut/qxtglobalshortcut_win.cpp
}

FORMS += MainWindow.ui SettingDialog.ui

TRANSLATIONS += ocrtool_zh.ts

RESOURCES += MainWindow.qrc

