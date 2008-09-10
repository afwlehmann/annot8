//
// main.cpp
// $Id$
//


#include "DBController.h"
#include "MainWindow.h"
#include "ChooseParticipantDlg.h"
#include <QApplication>
#include <QMessageBox>


using namespace hiwi;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    try {

        // Attempt to connect to the database.
        if (!DBController::instance()->connect("experiment.sql")) {
            QMessageBox::critical(0, QObject::tr("Error"),
                                  QObject::tr("Couldn't open the database!"));
            return 1;
        }

        // Let the user choose a participant.
        ChooseParticipantDlg cpDlg;
        if (QDialog::Accepted != cpDlg.exec() || !cpDlg.getParticipantID())
            return 0;

        // Now that the user has successfully chosen a participant, go on
        // and show the main window.
        MainWindow mw(cpDlg.getParticipantID());
        mw.show();
        mw.raise();

        // Let Qt do the main loop.
        app.exec();

    } catch (std::exception &ex) {
        QMessageBox::critical(0, QObject::tr("Error"), ex.what());
        return 1;

    } catch (...) {
        QMessageBox::critical(0, QObject::tr("Error"),
                              QObject::tr("An unknown error occured!"));
        return 1;
    }

    // Disconnect from the database.
    DBController::instance()->disconnect();

    return 0;
}
