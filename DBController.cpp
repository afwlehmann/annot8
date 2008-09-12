//
// DBController.cpp
// $Id$
//


#include "DBController.h"
#include "Movie.h"
#include "Participant.h"
#include "Samples.h"
#include <stdexcept>
#include <sstream>
#include <QtSql>


using namespace std;


namespace hiwi
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
                "CREATE TABLE IF NOT EXISTS samples ("
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
                "  suffix           TEXT NOT NULL,"
                "  first_frame      INTEGER NOT NULL,"
                "  last_frame       INTEGER NOT NULL,"
                "  num_digits       INTEGER NOT NULL,"
                "  description      TEXT"
                ");"
        );
        if (!query.exec())
            break;

        query.clear();
        query.prepare(
                "CREATE TABLE IF NOT EXISTS sender ("
                "  timestamp        REAL NOT NULL,"
                "  sender_id        INTEGER NOT NULL REFERENCES participants(id),"
                "  receiver_id      INTEGER NOT NULL REFERENCES participants(id),"
                "  UNIQUE(timestamp, sender_id, receiver_id)"
                ");"
        );
        if (!query.exec())
            break;

        query.clear();
        query.prepare(
                "CREATE TABLE IF NOT EXISTS attributes ("
                "  timestamp        REAL NOT NULL,"
                "  participant_id   INTEGER NOT NULL REFERENCES participants(id),"
                "  laughing         INTEGER"
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
    QSqlQuery query;

    do {
        // Begin a transaction.
        QSqlDatabase::database().transaction();

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
               << " FOR EACH ROW BEGIN " << ss.str() << " END;";
            if (!query.exec(QString::fromStdString(qs.str())))
                break;
            qs.str("CREATE TRIGGER IF NOT EXISTS ");
            qs << *it << "_before_update BEFORE UPDATE ON " << *it
               << " FOR EACH ROW BEGIN " << ss.str() << " END;";
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
               << it->second << " END;";
            if (!query.exec(QString::fromStdString(qs.str())))
                break;
        }
        if (query.lastError().isValid())
            break;

        // Create one trigger for checking the timestamp of an annotation.
        if (!query.exec("DROP TRIGGER IF EXISTS check_sender_timestamp_insert;") ||
            !query.exec(
                "CREATE TRIGGER IF NOT EXISTS check_sender_timestamp_insert "
                "BEFORE INSERT ON sender "
                "FOR EACH ROW BEGIN "
                "  SELECT RAISE(ROLLBACK, 'Invalid timestamp.')"
                "    WHERE NEW.timestamp < 0 OR NEW.timestamp > 1;"
                "END;") ||
            !query.exec("DROP TRIGGER IF EXISTS check_sender_timestamp_update;") ||
            !query.exec(
                "CREATE TRIGGER IF NOT EXISTS check_sender_timestamp_update "
                "BEFORE UPDATE ON sender "
                "FOR EACH ROW BEGIN "
                "  SELECT RAISE(ROLLBACK, 'Invalid timestamp.')"
                "    WHERE NEW.timestamp < 0 OR NEW.timestamp > 1; "
                "END;"))
        {
            break;
        }

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


std::vector<Movie *> DBController::getAvailableMovies() const
{
    std::vector<Movie *> result;

    QSqlQuery query;
    if (!query.exec("SELECT prefix, suffix, first_frame, last_frame, "
            "num_digits, description FROM movies"))
    {
        throw std::runtime_error("Unable to get the list of available movies!");
    }

    while (query.next()) {
        result.push_back(new Movie(
                query.value(0).toString().toStdString(),
                query.value(1).toString().toStdString(),
                query.value(2).toInt(),
                query.value(3).toInt(),
                query.value(4).toInt(),
                query.value(5).toString().toStdString())
        );
    }

    return result;
}


std::vector<Participant *> DBController::getParticipants() const
{
    std::vector<Participant *> result;

    QSqlQuery query;
    if (!query.exec("SELECT id, name, first_name, number FROM participants"))
        throw std::runtime_error("Unable to get the list of participants.");

    while (query.next()) {
        result.push_back(new Participant(
                query.value(0).toInt(),
                query.value(1).toString().toStdString(),
                query.value(2).toString().toStdString(),
                query.value(3).toInt())
        );
    }

    return result;
}


audio::Samples* DBController::getSamplesForParticipant(Participant *p) const
{
    QSqlQuery query;
    query.prepare("SELECT filename FROM samples WHERE participant_id = ?");
    query.addBindValue(p->id);
    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.next() ?
        new audio::Samples(query.value(0).toString().toStdString()) : 0;
}


void DBController::storeAnnotation(float timestamp, int senderID,
                                   const std::vector<int> &receiverIDs,
                                   bool laughing)
{
    QSqlQuery query;

    do {
        QSqlDatabase::database().transaction();

        query.clear();
        query.prepare("DELETE FROM sender WHERE sender_id = ? AND timestamp = ?");
        query.addBindValue(senderID);
        query.addBindValue(timestamp);
        if (!query.exec())
            break;

        for (std::vector<int>::const_iterator it = receiverIDs.begin();
            it != receiverIDs.end(); it++)
        {
            query.clear();
            query.prepare("INSERT INTO sender (timestamp, sender_id, receiver_id) "
                          "VALUES (?, ?, ?)");
            query.addBindValue(timestamp);
            query.addBindValue(senderID);
            query.addBindValue(*it);
            if (!query.exec())
                break;
        }
        if (query.lastError().isValid())
            break;

        query.clear();
        query.prepare("INSERT INTO attributes (timestamp, participant_id, laughing) "
                      "VALUES (?, ?, ?)");
        query.addBindValue(timestamp);
        query.addBindValue(senderID);
        query.addBindValue(laughing ? 1 : 0);
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


void DBController::getAnnotation(float timestamp, int senderID,
                                 std::vector<int> *receiverIDs,
                                 bool *laughing)
{
    QSqlQuery query;

    // Reset the annotations.
    receiverIDs->clear();
    *laughing = false;

    do {
        QSqlDatabase::database().transaction();

        // Get the IDs of the receivers.
        query.clear();
        query.prepare("SELECT receiver_id FROM sender WHERE sender_id = ? "
                      "AND timestamp = ?");
        query.addBindValue(senderID);
        query.addBindValue(timestamp);
        if (!query.exec())
            break;
        while (query.next())
            receiverIDs->push_back(query.value(0).toInt());

        // Get the stored attributes.
        query.clear();
        query.prepare("SELECT laughing FROM attributes WHERE participant_id = ? "
                      "AND timestamp = ?");
        query.addBindValue(senderID);
        query.addBindValue(timestamp);
        if (!query.exec())
            break;
        else if (query.next()) {
            *laughing = query.value(0).toBool();
        }
        query.finish();

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


} // namespace hiwi
