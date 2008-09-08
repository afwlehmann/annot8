//
// main.cpp
// $Id$
//


#include "DBController.h"
#include <iostream>
#include <QApplication>


using namespace std;
using namespace HiWi;


int main(int argc, char *argv[])
{
    // Attempt a connection to the database.
    if (!DBController::instance()->connect("experiment.sql")) {
        cerr << "Couldn't open the database!" << endl;
        return 1;
    }

    cout << "Initializing..." << endl;

    try {
        QApplication app(argc, argv);
        app.exec();
    } catch (std::exception &ex) {
        cerr << "An error occurred:" << endl
             << ex.what() << endl;
        return 1;
    } catch (...) {
        cerr << "An unknown error occurred!" << endl;
        return 1;
    }

    cout << "Shutting down..." << endl;

    // Disconnect from the database.
    DBController::instance()->disconnect();

    return 0;
}
