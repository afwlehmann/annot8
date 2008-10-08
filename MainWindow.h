//
// MainWindow.h
// $Id$
//


#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__


#include "ui_MainWindow.h"


namespace hiwi
{


// Forward declaration
struct Movie;
struct Participant;
namespace audio {
    class PlaybackThread;
    struct Samples;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int participantID, bool takeAlong);
    virtual ~MainWindow();


protected slots:
    /**
     * @name Menu
     * @{
     */


    /*
     * Event-handler for the "About" menu-item.
     */
    void on_actionAbout_triggered();


    /*
     * Event-handler for the "Quit" menu-item.
     */
    void on_actionQuit_triggered();


    /* *} */


    /**
     * @name Movie-related
     * @{
     */


    /**
     * Invoked whenever another movie is selected.
     */
    void on_twMovies_currentChanged(int index);


    /**
     * Event-handler for the current-frame slider. Invoked whenever the value
     * of the slider changes and thus a new frame ought to be selected.
     */
    void on_hsCurrentFrame_valueChanged(int value);


    /**
     * Event-handler for the "<"-button (previous frame).
     */
    void on_tbPrev_clicked();


    /**
     * Event-handler for the ">"-button (next frame).
     */
    void on_tbNext_clicked();


    /**
     * Event-handler for the 'Sync' button located at the ui's movie-related
     * portion.
     * Synchronizes the current frame with the current sample position.
     */
    void on_tbSyncMovie_clicked();


    void on_pbFlipbook_clicked();


    /* *} */


    /**
     * @name Samples-related
     * @{
     */


    /**
     * Event-handler for the '+' button. Zooms into the samples.
     */
    void on_tbZoomIn_clicked();


    /**
     * Event-handler for the '-' button. Zooms out of the samples.
     */
    void on_tbZoomOut_clicked();


    /**
     * Event-handler for the samples-position horizontal scrollbar.
     */
    void on_hsbSamples_valueChanged(int);


    /**
     * Event-handler for the 'Play' (and also 'Pause') button. Updates the ui
     * accordingly and starts the playback of the samples.
     */
    void on_pbPlay_clicked();


    /**
     * Invoked whenever the user clicks on a point within the samples preview
     * canvas. The current samples position is then updated to the selected
     * position.
     */
    void on_spvCanvas_posClicked(float);


    /**
     * Event-handler for the playbackPosChanged-signal of the playback thread.
     * Is called fairly often during playback (at about 10Hz), for example in
     * order to update the marker position of the samples.
     */
    void xx_on_pbThread_playbackPosChanged(float);


    /**
     * Invoked whenver the playback thread stop playing.
     */
    void xx_on_pbThread_finished();


    /**
     * Event-handler for the 'Sync' button located at the ui's samples-related
     * portion.
     * Synchronizes the current sample position with the current frame.
     */
    void on_tbSyncSamples_clicked();


    /* *} */


    /**
     * Event-handler for the "Reset"-button. Rereads the annotations for
     * the current frame and sets the UI's elements accordingly.
     */
    void on_pbReset_clicked();


    /**
     * Event-handler for the "Save"-button. Stores the annotations of the
     * current frame and resets the `window modified'-flag.
     */
    void on_pbSave_clicked();


    /**
     * Event-handler for the "Save and continue"-button. Stores the annotations
     * of the current frame, resets the `window modified'-flag and displays
     * the next frame. Eventually synchronizes the samples position with the
     * new frame position.
     */
    void on_pbSaveAndContinue_clicked();


protected:
    /**
     * Invoked whenever the user presses a key.
     */
    virtual void keyPressEvent(QKeyEvent *);


    /**
     * Selects the previous movie of the movie tabwidget.
     */
    void selectPreviousMovie();


    /**
     * Selects the next movie of the movie tabwidget.
     */
    void selectNextMovie();


    /**
     * Swaps between the current and the last selected movie of the movie
     * tabwidget.
     */
    void swapMovies();


    /**
     * Gets the available movies and provides the ui with corresponding movie
     * widgets.
     * Only called during construction.
     */
    void setupMovies();


    /**
     * Gets the available participants and provides the ui with corresponding
     * list widget items. Also updates the information about the active, i.e.
     * the one to be worked on, participant.
     */
    void setupParticipants(int participantID);


    /**
     * Gets the samples linked to the active participant. Updates the samples
     * preview canvas and creates the playback thread, too.
     */
    void setupSamples();


    /**
     * Maps the given value from [0, numFrames-1] to [0, 1].
     */
    float positionForValue(int value) const;


    /**
     * Returns the current position mapped to [0, 1].
     */
    float currentPosition() const;


    /**
     * Returns the current timestamp in milliseconds.
     */
    int currentTimestamp() const;


    /**
     * Returns the current frame number.
     */
    int currentFrame() const;


    /**
     * Invoked when, for example, another movie is selected. Although actually
     * the same length of all movies is ensured during construction of the
     * main window, this function makes sure that the frame-related horizontal
     * scrollbar range fits the number of frames of the currently selected
     * movie. On the one hand this is a safety measure, and on the other hand
     * this is a convenience function.
     */
    void updateScrollBarRange();


    /**
     * Enables or disables a whole bunch of ui items according to the current
     * playback state.
     */
    void enableUI(bool playing);


    /**
     * Clears all annotations, i.e. resets the corresponding widgets to their
     * default state.
     */
    void clearAnnotations();


    /**
     * Load all annotations for the current frame. If `takeAlong' is specified
     * as true, then only reset and/or load the annotations as long as
     * annotations are available for the current frame.
     * @param  takeAlong        whether to take along the current annotations
     */
    void loadAnnotations(bool takeAlong = false);


    /**
     * Go to the given frame.
     * @param  frame            the number of the new frame
     */
    void goToFrame(int frame);


private:
    Ui::MainWindow          _ui;
    std::vector<Movie *>    _movies;
    Participant             *_participant;
    audio::Samples          *_samples;
    audio::PlaybackThread   *_pbThread;
    bool                    _playing;
    bool                    _flipping;
    bool                    _takeAlong;

    // The following array is used as a ringbuffer to distinguish between the
    // current and the former selected movie index.
    int                     _lastSelectedMovieIdx[2];
    int                     _lastSelectedMovieIdxPos;
};


} // namespace hiwi


#endif
