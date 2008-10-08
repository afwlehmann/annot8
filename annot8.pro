CONFIG += qt thread resources
QT += sql gui

SOURCES = ChooseParticipantDlg.cpp \
          DBController.cpp \
          DBSetupMoviesWidget.cpp \
          DBSetupParticipantsWidget.cpp \
          DBSetupSamplesWidget.cpp \
          DBSetupWizard.cpp \
          FileNameDelegate.cpp \
          FileNameWidget.cpp \
          ImageProducer.cpp \
          MainWindow.cpp \
          MovieWidget.cpp \
          PlaybackThread.cpp \
          Samples.cpp \
          SamplesPreviewCanvas.cpp \
          audio.cpp \
          main.cpp

HEADERS = AboutDlg.h \
          Attributes.h \
          ChooseParticipantDlg.h \
          DBController.h \
          DBSetupMoviesWidget.h \
          DBSetupParticipantsWidget.h \
          DBSetupSamplesWidget.h \
          DBSetupWizard.h \
          FileNameDelegate.h \
          FileNameWidget.h \
          ImageProducer.h \
          MainWindow.h \
          Movie.h \
          MovieWidget.h \
          Participant.h \
          PlaybackThread.h \
          Samples.h \
          SamplesPreviewCanvas.h \
          audio.h

FORMS = AboutDlg.ui \
        ChooseParticipantDlg.ui \
        DBSetupMoviesWidget.ui \
        DBSetupParticipantsWidget.ui \
        DBSetupSamplesWidget.ui \
        FileNameWidget.ui \
        MainWindow.ui

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
    #CONFIG -= app_bundle
    INCLUDEPATH += /sw/include
    LIBS += -L/sw/lib
    CONFIG(debug, debug|release) {
        MY_BUNDLE_FRAMEWORKS.files = /usr/local/Trolltech/current/lib/libQtCore_debug.4.dylib \
                                     /usr/local/Trolltech/current/lib/libQtGui_debug.4.dylib \
                                     /usr/local/Trolltech/current/lib/libQtSql_debug.4.dylib
    }
    CONFIG(release, debug|release) {
        MY_BUNDLE_FRAMEWORKS.files = /usr/local/Trolltech/current/lib/libQtCore.4.dylib \
                                     /usr/local/Trolltech/current/lib/libQtGui.4.dylib \
                                     /usr/local/Trolltech/current/lib/libQtSql.4.dylib
    }
    MY_BUNDLE_FRAMEWORKS.files += /sw/lib/libSDL.dylib \
                                  /sw/lib/libSDL_sound.dylib \
                                  /sw/lib/libsmpeg.dylib \
	                              /sw/lib/libmikmod.dylib \
	                              /sw/lib/libvorbis.dylib \
	                              /sw/lib/libvorbisfile.dylib \
	                              /sw/lib/libvorbisenc.dylib \
	                              /sw/lib/libogg.dylib \
	                              /sw/lib/libspeex.dylib
    MY_BUNDLE_FRAMEWORKS.path = Frameworks
    MY_BUNDLE_RESOURCES.files = res/mac/experiment.sql
    MY_BUNDLE_RESOURCES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += MY_BUNDLE_FRAMEWORKS MY_BUNDLE_RESOURCES
    CONFIG(app_bundle):DEFINES += OSX_APP_BUNDLE
}

win32 {
    # Add CONFIG += console for messages on std(out|err).
    CONFIG += windows embed_manifest_exe
    LIBS += SDL.lib SDL_sound.lib
}

unix {
    LIBS += -lSDL -lSDL_sound
}

UI_DIR = $$IMDIR
MOC_DIR = $$IMDIR
OBJECTS_DIR = $$IMDIR
RCC_DIR = $$IMDIR

TARGET = $$IMDIR/annot8
