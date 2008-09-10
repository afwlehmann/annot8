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
struct Participant;


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
     * Event-handler for the "Preferences" menu-item.
     */
    void on_actionPreferences_triggered();


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


    void on_pbReset_clicked();


    void on_pbSave_clicked();


    void on_pbSaveAndContinue_clicked();


    /* *} */


private:
    /**
     * Maps the given value from [0, numFrames-1] to [0, 1].
     */
    float positionForValue(int value) const;


    void setRange(int min, int max);


    Ui::MainWindow  _ui;
    Participant     *_participant;
};


} // namespace hiwi


#endif
