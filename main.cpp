//
// main.cpp
// $Id$
//
// Copyright 2008 by Alexander Lehmann <lehmanna@in.tum.de>
//
// This file is part of annot8.
// 
// annot8 is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
// 
// annot8 is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with
// annot8.  If not, see <http://www.gnu.org/licenses/>.
//


#include "DBController.h"
#include "MainWindow.h"
#include "ChooseParticipantDlg.h"
#include "audio.h"
#include "DBSetupWizard.h"

#include <iostream>

#include <QApplication>
#include <QMessageBox>


using namespace annot8;
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
#ifndef OSX_APP_BUNDLE
        if (!DBController::instance()->connect("experiment.sql")) {
#else
        const QString path = app.applicationDirPath();
        app.addLibraryPath(QString("%1/../Resources/plugins").arg(path));
        QString fileName = QString("%1/../Resources/experiment.sql").arg(path);
        if (!DBController::instance()->connect(fileName.toStdString())) {
#endif
            QMessageBox::critical(0,
                                  QObject::tr("Error"),
                                  QObject::tr("Couldn't open the database!"));
            return 1;
        }

        // Parse the command-line arguments, but remove the first argument
        // beforehand because it is only the application's executable name.
        QList<QString> args = app.arguments();
        if (!args.empty()) args.pop_front();
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
