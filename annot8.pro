CONFIG += qt thread
QT += sql gui

SOURCES = main.cpp DBController.cpp

HEADERS = DBController.h

CONFIG(debug, debug|release) {
    IMDIR = build/debug
    DEFINES += DEBUG
}

CONFIG(release, debug|release) {
    IMDIR = build/release
}

mac {
    CONFIG -= app_bundle
}

win32 {
    # Add CONFIG += console for messages on std(out|err).
    CONFIG += windows embed_manifest_exe
}

UI_DIR = $$IMDIR
MOC_DIR = $$IMDIR
OBJECTS_DIR = $$IMDIR
RCC_DIR = $$IMDIR

TARGET = $$IMDIR/annot8
