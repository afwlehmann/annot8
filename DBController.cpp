//
// DBController.cpp
// $Id$
//


#include <QtSql>


#include "DBController.h"


namespace HiWi
{


DBController* DBController::_instance = 0;


DBController::DBController()
{
    // Intentionally left blank.
}


DBController::~DBController()
{
    if (QSqlDatabase::database().isOpen())
        disconnect();
}


DBController* DBController::instance()
{
    if (!_instance)
        _instance = new DBController();

    return _instance;
}


bool DBController::connect(const std::string &fileName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fileName.c_str());
    return db.open();
}


void DBController::disconnect()
{
    QSqlDatabase::database().close();
}


} // namespace HiWi
