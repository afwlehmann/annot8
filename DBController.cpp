//
// DBController.cpp
// $Id$
//


#include "DBController.h"
#include <stdexcept>
#include <sstream>
#include <QtSql>


using namespace std;


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

    // Open the database.
    if (!db.open())
        return false;

    // Set up the database if the connection attempt was successful.
    setupTables();
    setupTriggers();

    return true;

}


void DBController::disconnect()
{
    QSqlDatabase::database().close();
}


void DBController::setupTables()
{
    // Begin a transaction and set up the tables and triggers.
    QSqlDatabase::database().transaction();
    QSqlQuery query;

    do {
        query.clear();
        query.prepare(
                "CREATE TABLE IF NOT EXISTS participants ("
                "  id               INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                "  name             TEXT NOT NULL,"
                "  first_name       TEXT,"
                "  number           INTEGER"
                ");"
        );
        if (!query.exec())
            break;

        query.clear();
        query.prepare(
                "CREATE TABLE IF NOT EXISTS sounds ("
                "  participant_id   INTEGER NOT NULL REFERENCES participants(id),"
                "  filename         TEXT NOT NULL"
                ");"
        );
        if (!query.exec())
            break;

        query.clear();
        query.prepare(
                "CREATE TABLE IF NOT EXISTS movies ("
                "  prefix           TEXT NOT NULL,"
                "  first_frame      INTEGER NOT NULL,"
                "  resolution       REAL NOT NULL,"
                "  description      TEXT"
                ");"
        );
        if (!query.exec())
            break;

        query.clear();
        query.prepare(
                "CREATE TABLE IF NOT EXISTS sender ("
                "  timestamp        INTEGER NOT NULL,"
                "  sender_id        INTEGER NOT NULL REFERENCES participants(id),"
                "  receiver_id      INTEGER NOT NULL REFERENCES participants(id)"
                ");"
        );
        if (!query.exec())
            break;

        // Finalize the transaction.
        if (!QSqlDatabase::database().commit())
            break;

        // Everything's fine, return now.
        return;
    } while (false);

    // Once we get to this point, an error has occured. Let's throw an
    // exception.
    throw std::runtime_error(query.lastError().text().toStdString());
}


void DBController::setupTriggers()
{
    // Begin a transaction.
    QSqlDatabase::database().transaction();
    QSqlQuery query;

    do {
        // At first, get a list of all tables.
        vector<string> tables;
        query.clear();
        if (!query.exec("SELECT tbl_name FROM sqlite_master WHERE "
                        "type = 'table' AND name <> 'sqlite_sequence'"))
        {
            break;
        }
        while (query.next()) {
            QSqlRecord r = query.record();
            tables.push_back(r.value(0).toString().toStdString());
        }

        // Determine the respective foreign key constraints and create the
        // corresponding INSERT- and UPDATE-triggers. See the SQLite
        // documentation for further explanations of the PRAGMA directive.
        map<string, string> cascades;
        for (vector<string>::const_iterator it = tables.begin();
            it != tables.end(); it++)
        {
            ostringstream qs(ios_base::app | ios_base::ate);
            qs << "PRAGMA foreign_key_list(" << *it << ");";
            if (!query.exec(QString::fromStdString(qs.str())))
                break;

            vector<string> dest, from, to;
            while (query.next()) {
                QSqlRecord r = query.record();
                dest.push_back(r.value(2).toString().toStdString());
                from.push_back(r.value(3).toString().toStdString());
                to.push_back(r.value(4).toString().toStdString());
            }

            // Dropping is only neccessary when the tables' layout has changed.
            // Please do not delete the following lines, but leave them
            // commented out instead.
            qs.str("DROP TRIGGER IF EXISTS ");
            qs << *it << "_before_insert";
            if (!query.exec(QString::fromStdString(qs.str())))
                break;
            qs.str("DROP TRIGGER IF EXISTS ");
            qs << *it << "_before_update";
            if (!query.exec(QString::fromStdString(qs.str())))
                break;
            qs.str("DROP TRIGGER IF EXISTS ");
            qs << *it << "_after_delete";
            if (!query.exec(QString::fromStdString(qs.str())))
                break;

            if (dest.empty())
                continue;

            // Prepare the SELECT statements for the INSERT- and UPDATE-triggers
            // and build the cascades map for the DELETE-trigger.
            stringstream ss;
            for (unsigned int i = 0; i < dest.size(); ++i) {
                ss << "SELECT RAISE(ROLLBACK, 'Invalid " << from.at(i) << ".') "
                   << "WHERE (SELECT " << to.at(i) << " FROM " << dest.at(i)
                   << " WHERE " << to.at(i) << " = NEW." << from.at(i) << ") "
                   << "IS NULL;";

                stringstream cs(cascades[dest.at(i)],
                                ios_base::out | ios_base::app | ios_base::ate);
                cs << "DELETE FROM " << *it
                   << " WHERE " << from.at(i) << " = OLD." << to.at(i) << ";";
                cascades[dest.at(i)] = cs.str();
            }

            // Create the INSERT- and UPDATE-triggers.
            qs.str("CREATE TRIGGER IF NOT EXISTS ");
            qs << *it << "_before_insert BEFORE INSERT ON " << *it
               << " FOR EACH ROW BEGIN " << ss.str() << " END";
            if (!query.exec(QString::fromStdString(qs.str())))
                break;
            qs.str("CREATE TRIGGER IF NOT EXISTS ");
            qs << *it << "_before_update BEFORE UPDATE ON " << *it
               << " FOR EACH ROW BEGIN " << ss.str() << " END";
            if (!query.exec(QString::fromStdString(qs.str())))
                break;
        }
        if (query.lastError().isValid())
            break;

        // Create the DELETE-triggers.
        for (map<string, string>::const_iterator it = cascades.begin();
            it != cascades.end(); it++)
        {
            ostringstream qs;
            qs << "CREATE TRIGGER IF NOT EXISTS " << it->first << "_after_delete "
               << "AFTER DELETE ON " << it->first << " FOR EACH ROW BEGIN "
               << it->second << " END";
            if (!query.exec(QString::fromStdString(qs.str())))
                break;
        }
        if (query.lastError().isValid())
            break;

        // Finalize the transaction.
        if (!QSqlDatabase::database().commit())
            break;

        // Everything's fine, return now.
        return;
    } while (false);

    // Once we get to this point, an error has occured. Let's throw an
    // exception.
    throw std::runtime_error(query.lastError().text().toStdString());
}


} // namespace HiWi
