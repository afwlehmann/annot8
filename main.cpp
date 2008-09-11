//
// main.cpp
// $Id$
//


#include "DBController.h"
#include "MainWindow.h"
#include "ChooseParticipantDlg.h"

#include <iostream>

#include <QApplication>
#include <QMessageBox>
#include <QFile>


#define DB_FILENAME "experiment.sql"


using namespace hiwi;
using namespace std;


void parseCmdLine(const QList<QString> &args)
{
    foreach (const QString &arg, args) {
        if (!arg.compare("--initdb", Qt::CaseInsensitive)) {
            // Delete the old file, then create a new database and eventually
            // exit.
            cout << "Removing database " << DB_FILENAME << "." << endl;
            QFile::remove(DB_FILENAME);
            cout << "Initializing new database " << DB_FILENAME << "." << endl;
            DBController::instance()->connect(DB_FILENAME);
            DBController::instance()->disconnect();
            cout << "Exiting." << endl;
            exit(0);
        } else if (!arg.compare("--help", Qt::CaseInsensitive)) {
            cout << "Usage: annot8 [options]" << endl
                 << "  --initdb    Remove the old database file and initialize a new one file." << endl
                 << "  --help      Display this message." << endl;
            exit(1);
        } else {
            cerr << "Unknown option: " << arg.toStdString() << endl;
            exit(1);
        }
    }
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    try {
        // Parse the command-line arguments, but remove the first argument
        // beforehand because it is only the application's executable name.
        QList<QString> args = app.arguments();
        args.pop_front();
        parseCmdLine(args);

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

    } catch (exception &ex) {
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
