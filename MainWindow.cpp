//
// MainWindow.cpp
// $Id$
//


#include "MainWindow.h"
#include <QMessageBox>


namespace hiwi {


MainWindow::MainWindow()
{
    _ui.setupUi(this);
}


MainWindow::~MainWindow()
{
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("Unavailable"), tr("Not yet implemented."));
}


void MainWindow::on_actionPreferences_triggered()
{
    QMessageBox::information(this, tr("Unavailable"), tr("Not yet implemented."));
}


void MainWindow::on_actionQuit_triggered()
{
    close();
}


} // namespace hiwi
