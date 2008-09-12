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
class PlaybackThread;
namespace audio { class Samples; }


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int participantID);
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
     * @name Widgets
     * @{
     */


    /**
     * Invoked whenever another movie is selected.
     */
    void on_twMovies_currentChanged(int index);


    /**
     * Event-handler for the current-frame slider.
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
     * Invoked whenever the user clicks on a list widget item.
     */
    void on_lwReceivers_itemClicked(QListWidgetItem *);


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
     * the next frame.
     */
    void on_pbSaveAndContinue_clicked();


    void on_tbZoomIn_clicked();
    void on_tbZoomOut_clicked();
    void on_hsbSamples_valueChanged(int);
    void on_pbPlay_clicked();
    void on_pbPause_clicked();
    void on_spvCanvas_posClicked(float);
    void xx_on_pbThread_playbackPosChanged(float);
    void on_tbSyncSamples_clicked();
    void on_tbSyncMovie_clicked();


    /* *} */


private:
    void setupMovies();
    void setupParticipants(int participantID);
    void setupSamples();

    /**
     * Maps the given value from [0, numFrames-1] to [0, 1].
     */
    float positionForValue(int value) const;


    void setFramesRange(int min, int max);
    void updateScrollBarRange();
    void enableUI(bool playing);


    Ui::MainWindow          _ui;
    std::vector<Movie *>    _movies;
    Participant             *_participant;
    audio::Samples          *_samples;
    PlaybackThread          *_pbThread;
};


} // namespace hiwi


#endif
