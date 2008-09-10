CONFIG += qt thread
QT += sql gui

SOURCES = ChooseParticipantDlg.cpp \
          DBController.cpp \
          MainWindow.cpp \
          MovieWidget.cpp \
          PlaybackThread.cpp \
          SamplesPreviewCanvas.cpp \
          SamplesPreviewWidget.cpp \
          Sound.cpp \
          audio.cpp \
          main.cpp

HEADERS = ChooseParticipantDlg.h \
          DBController.h \
          MainWindow.h \
          Movie.h \
          MovieWidget.h \
          Participant.h \
          PlaybackThread.h \
          SamplesPreviewCanvas.h \
          SamplesPreviewWidget.h \
          Sound.h \
          audio.h

FORMS = ChooseParticipantDlg.ui MainWindow.ui

CONFIG(debug, debug|release) {
    IMDIR = build/debug
    DEFINES += DEBUG
}

CONFIG(release, debug|release) {
    IMDIR = build/release
}

mac {
    CONFIG -= app_bundle
    INCLUDEPATH += /sw/include
    LIBS += -L/sw/lib
}

win32 {
    # Add CONFIG += console for messages on std(out|err).
    CONFIG += windows embed_manifest_exe
}

unix {
    LIBS += -lSDL -lSDL_sound
}

UI_DIR = $$IMDIR
MOC_DIR = $$IMDIR
OBJECTS_DIR = $$IMDIR
RCC_DIR = $$IMDIR

TARGET = $$IMDIR/annot8
