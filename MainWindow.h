//
// MainWindow.h
// $Id$
//


#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__


#include "ui_MainWindow.h"


namespace HiWi
{


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();


private:
    Ui::MainWindow  _ui;
};


} // namespace HiWi


#endif
