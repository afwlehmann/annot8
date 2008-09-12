CONFIG += qt thread resources
QT += sql gui

SOURCES = ChooseParticipantDlg.cpp \
          DBController.cpp \
          MainWindow.cpp \
          MovieWidget.cpp \
          PlaybackThread.cpp \
          Samples.cpp \
          SamplesPreviewCanvas.cpp \
          audio.cpp \
          main.cpp

HEADERS = AboutDlg.h \
          ChooseParticipantDlg.h \
          DBController.h \
          MainWindow.h \
          Movie.h \
          MovieWidget.h \
          Participant.h \
          PlaybackThread.h \
          Samples.h \
          SamplesPreviewCanvas.h \
          audio.h

FORMS = AboutDlg.ui ChooseParticipantDlg.ui MainWindow.ui

RESOURCES = annot8.res

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
