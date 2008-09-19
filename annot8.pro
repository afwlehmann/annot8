CONFIG += qt thread resources
QT += sql gui

SOURCES = ChooseParticipantDlg.cpp \
          DBController.cpp \
          ImageProducer.cpp \
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
          ImageProducer.h \
          MainWindow.h \
          Movie.h \
          MovieWidget.h \
          Participant.h \
          PlaybackThread.h \
          Samples.h \
          SamplesPreviewCanvas.h \
          audio.h

FORMS = AboutDlg.ui ChooseParticipantDlg.ui MainWindow.ui

RESOURCES = annot8.qrc

CONFIG(debug, debug|release) {
    IMDIR = build/debug
    DEFINES += DEBUG
    win32:DESTDIR = build/debug
}

CONFIG(release, debug|release) {
    IMDIR = build/release
    win32:DESTDIR = build/release
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
