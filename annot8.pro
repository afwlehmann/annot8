CONFIG += qt thread resources x86
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
    CONFIG(app_bundle) {
        message("Building application bundle.")
        DEFINES += OSX_APP_BUNDLE
        MY_BUNDLE_RESOURCES.files = res/unix/experiment.sql
        MY_BUNDLE_RESOURCES.path = Contents/Resources
        CONFIG(debug, debug|release) {
            IMF_PLUGINS.files = $$[QT_INSTALL_PLUGINS]/imageformats/libqgif.dylib \
                                $$[QT_INSTALL_PLUGINS]/imageformats/libqjpeg.dylib \
                                $$[QT_INSTALL_PLUGINS]/imageformats/libqtiff.dylib
            SQL_PLUGINS.files = $$[QT_INSTALL_PLUGINS]/sqldrivers/libqsqlite.dylib
            # IMF_PLUGINS.files = $$[QT_INSTALL_PLUGINS]/imageformats/libqgif_debug.dylib \
            #                     $$[QT_INSTALL_PLUGINS]/imageformats/libqjpeg_debug.dylib \
            #                     $$[QT_INSTALL_PLUGINS]/imageformats/libqtiff_debug.dylib
            # SQL_PLUGINS.files = $$[QT_INSTALL_PLUGINS]/sqldrivers/libqsqlite_debug.dylib
            QMAKE_POST_LINK += "./finalize_app build/debug/annot8.app"
        }
        CONFIG(release, debug|release) {
            IMF_PLUGINS.files = $$[QT_INSTALL_PLUGINS]/imageformats/libqgif.dylib \
                                $$[QT_INSTALL_PLUGINS]/imageformats/libqjpeg.dylib \
                                $$[QT_INSTALL_PLUGINS]/imageformats/libqtiff.dylib
            SQL_PLUGINS.files = $$[QT_INSTALL_PLUGINS]/sqldrivers/libqsqlite.dylib
            QMAKE_POST_LINK += "./finalize_app build/release/annot8.app"
        }
        IMF_PLUGINS.path = Contents/Resources/plugins/imageformats
        SQL_PLUGINS.path = Contents/Resources/plugins/sqldrivers
        QMAKE_BUNDLE_DATA += MY_BUNDLE_RESOURCES IMF_PLUGINS SQL_PLUGINS
    }
    ICON = res/mac/annot8.icns
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
