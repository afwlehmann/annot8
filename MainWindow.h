//
// MainWindow.h
// $Id$
//


#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__


#include "ui_MainWindow.h"


namespace hiwi
{


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();


protected slots:
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


private:
    Ui::MainWindow  _ui;
};


} // namespace hiwi


#endif
