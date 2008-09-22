//
// main.cpp
// $Id$
//


#include "DBController.h"
#include "MainWindow.h"
#include "ChooseParticipantDlg.h"
#include "audio.h"
#include "DBSetupWizard.h"

#include <iostream>

#include <QApplication>
#include <QMessageBox>
#include <QFile>


#define DB_FILENAME "experiment.sql"


using namespace hiwi;
using namespace std;


void displayUsageAndExit()
{
    cout << "Usage: annot8 [options]" << endl
         << "  --initdb          Remove the old database file and initialize a new one file." << endl
         << "  --no-take-along   Don't take along any annotations." << endl
         << "  --help            Display this message." << endl;
    exit(1);
}


void parseCmdLine(const QList<QString> &args, bool *takeAlong)
{
    *takeAlong = true;

    foreach (const QString &arg, args) {
        if (!arg.compare("--initdb", Qt::CaseInsensitive)) {
            DBSetupWizard wizard;
            wizard.raise();
            if (QDialog::Accepted != wizard.exec())
                exit(1);
        } else if (!arg.compare("--no-take-along", Qt::CaseInsensitive)) {
            *takeAlong = false;
        } else if (!arg.compare("--help", Qt::CaseInsensitive)) {
            displayUsageAndExit();
        } else {
            cerr << "Unknown option: " << arg.toStdString() << endl << endl;
            displayUsageAndExit();
        }
    }
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Initialize SDL and SDL_sound.
    if (!audio::initialize()) {
        QMessageBox::critical(0, QObject::tr("Error"),
                QObject::tr("Unable to initialize SDL and/or SDL_sound."));
        exit(1);
    }

    try {
        bool takeAlong;

        // Attempt to connect to the database.
        if (!DBController::instance()->connect("experiment.sql")) {
            QMessageBox::critical(0,
                                  QObject::tr("Error"),
                                  QObject::tr("Couldn't open the database!"));
            return 1;
        }

        // Parse the command-line arguments, but remove the first argument
        // beforehand because it is only the application's executable name.
        QList<QString> args = app.arguments();
        args.pop_front();
        parseCmdLine(args, &takeAlong);

        // Let the user choose a participant.
        ChooseParticipantDlg cpDlg;
        if (QDialog::Accepted != cpDlg.exec() || !cpDlg.getParticipantID())
            return 0;

        // Now that the user has successfully chosen a participant, go on
        // and show the main window.
        MainWindow mw(cpDlg.getParticipantID(), takeAlong);
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

    // Perform a clean shutdown of SDL and SDL_sound.
    audio::shutdown();

    return 0;
}
