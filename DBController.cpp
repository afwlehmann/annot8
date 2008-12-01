//
// DBController.cpp
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
#include "Movie.h"
#include "Participant.h"
#include "Samples.h"
#include "Attributes.h"
#include <stdexcept>
#include <sstream>
#include <QtSql>


using namespace std;


namespace annot8
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
        if (!query.exec(
                "CREATE TABLE IF NOT EXISTS participants ("
                "  id               INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                "  name             TEXT NOT NULL,"
                "  first_name       TEXT,"
                "  number           INTEGER"
                ");"))
        { break; }

        if (!query.exec(
                "CREATE TABLE IF NOT EXISTS samples ("
                "  participant_id   INTEGER NOT NULL REFERENCES participants(id),"
                "  filename         TEXT NOT NULL"
                ");"))
        { break; }

        if (!query.exec(
                "CREATE VIEW IF NOT EXISTS pinfo AS"
                " SELECT id, name || COALESCE(', ' || first_name, '') "
                "   || COALESCE(' (' || number || ')', '') AS name_info"
                " FROM participants;"))
        { break; }

        if (!query.exec(
                "CREATE TABLE IF NOT EXISTS movies ("
                "  prefix           TEXT NOT NULL,"
                "  suffix           TEXT NOT NULL,"
                "  first_frame      INTEGER NOT NULL,"
                "  last_frame       INTEGER NOT NULL,"
                "  ms_per_frame     INTEGER NOT NULL,"
                "  num_digits       INTEGER NOT NULL,"
                "  description      TEXT"
                ");"))
        { break; }

        if (!query.exec(
                "CREATE TABLE IF NOT EXISTS sender ("
                "  timestamp        INTEGER NOT NULL,"
                "  sender_id        INTEGER NOT NULL REFERENCES participants(id),"
                "  receiver_id      INTEGER NOT NULL REFERENCES participants(id),"
                "  UNIQUE(timestamp, sender_id, receiver_id)"
                ");") ||
            !query.exec(
                "CREATE INDEX IF NOT EXISTS sender_custom_index "
                " ON sender (timestamp, sender_id);"))
        { break; }

        if (!query.exec(
                "CREATE TABLE IF NOT EXISTS attributes ("
                "  timestamp        INTEGER NOT NULL,"
                "  participant_id   INTEGER NOT NULL REFERENCES participants(id),"
                "  speaking         INTEGER,"
                "  laughing         INTEGER,"
                "  UNIQUE(timestamp, participant_id)"
                ");") ||
            !query.exec(
                "CREATE INDEX IF NOT EXISTS attributes_custom_index "
                " ON attributes (timestamp, participant_id);"))
        { break; }

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
        // corresponding INSERT- and UPDATE-triggers for the children. Also,
        // create DELETE- and UPDATE-triggers for the parents. See the SQLite
        // documentation for further explanations of the PRAGMA directive.
        map<string, string> parentDeletes, parentUpdates;
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
            qs.str("DROP TRIGGER IF EXISTS ");
            qs << *it << "_after_update";
            if (!query.exec(QString::fromStdString(qs.str())))
                break;

            if (dest.empty())
                continue;

            // Prepare the SELECT statements for the INSERT- and UPDATE-triggers
            // of the children and build the map containing the DELETE- and
            // UPDATE-triggers of the parents.
            stringstream ss;
            for (unsigned int i = 0; i < dest.size(); ++i) {
                // The following SQL statement will be used to assure that
                // children only refer to valid parent columns on INSERT and
                // UPDATE.
                ss << "SELECT RAISE(ROLLBACK, 'Invalid " << from.at(i) << ".') "
                   << "WHERE (SELECT " << to.at(i) << " FROM " << dest.at(i)
                   << " WHERE " << to.at(i) << " = NEW." << from.at(i) << ") "
                   << "IS NULL;";

                // The following SQL statement will be used to assure deletion
                // of children on parent DELETE.
                stringstream pdss(parentDeletes[dest.at(i)],
                                  ios_base::out | ios_base::app | ios_base::ate);
                pdss << "DELETE FROM " << *it
                   << " WHERE " << from.at(i) << " = OLD." << to.at(i) << ";";
                parentDeletes[dest.at(i)] = pdss.str();

                // The following SQL statement will be used to update children
                // columns accordingly on parent UPDATE.
                stringstream puss(parentUpdates[dest.at(i)],
                                  ios_base::out | ios_base::app | ios_base::ate);
                puss << "UPDATE " << *it
                     << " SET " << from.at(i) << " = NEW." << to.at(i)
                     << " WHERE " << from.at(i) << " = OLD." << to.at(i) << ";";
                parentUpdates[dest.at(i)] = puss.str();
            }

            // Create the children INSERT- and UPDATE-triggers.
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

        // Create the parents' DELETE-triggers.
        for (map<string, string>::const_iterator it = parentDeletes.begin();
            it != parentDeletes.end(); it++)
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

        // Create the parents' UPDATE-triggers.
        for (map<string, string>::const_iterator it = parentUpdates.begin();
            it != parentUpdates.end(); it++)
        {
            ostringstream qs;
            qs << "CREATE TRIGGER IF NOT EXISTS " << it->first << "_after_update "
               << "AFTER UPDATE ON " << it->first << " FOR EACH ROW BEGIN "
               << it->second << " END;";
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


std::vector<Movie *> DBController::getAvailableMovies() const
{
    std::vector<Movie *> result;

    QSqlQuery query;
    if (!query.exec("SELECT prefix, suffix, first_frame, last_frame, "
            "ms_per_frame, num_digits, description FROM movies"))
    {
        throw std::runtime_error("Unable to get the list of available movies!");
    }

    while (query.next()) {
        result.push_back(new Movie(
                query.value(0).toString().toStdString(),    // prefix
                query.value(1).toString().toStdString(),    // suffix
                query.value(2).toInt(),                     // first_frame
                query.value(3).toInt(),                     // last_frame
                query.value(4).toInt(),                     // ms_per_frame,
                query.value(5).toInt(),                     // num_digits
                query.value(6).toString().toStdString())    // descripition
        );
    }

    return result;
}


std::vector<Participant *> DBController::getParticipants(bool withSamplesOnly) const
{
    std::vector<Participant *> result;

    QSqlQuery query;
    if (withSamplesOnly) {
        if (!query.exec("SELECT id, name, first_name, number FROM participants "
                        "JOIN samples ON samples.participant_id = id"))
            throw std::runtime_error("Unable to get the list of participants.");
    } else {
        if (!query.exec("SELECT id, name, first_name, number FROM participants"))
            throw std::runtime_error("Unable to get the list of participants.");
    }

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


void DBController::getParticipantsInfo(std::map<int, std::string> *info)
{
    QSqlQuery query;
    if (!query.exec("SELECT id, name_info FROM pinfo"))
        throw std::runtime_error(query.lastError().text().toStdString());

    info->clear();
    while (query.next()) {
        info->insert(
            std::pair<int, string>(query.value(0).toInt(),
                                   query.value(1).toString().toStdString())
        );
    }
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


void DBController::storeAvailableSamples(std::map<int, std::string> &samples)
{
    QSqlQuery query;

    do {
        // Begin a transaction.
        QSqlDatabase::database().transaction();

        if (!query.exec("DELETE FROM samples"))
            break;

        std::map<int, std::string>::const_iterator it;
        for (it = samples.begin(); it != samples.end(); it++) {
            query.clear();
            query.prepare("INSERT INTO samples (participant_id, filename) "
                          "VALUES (?, ?)");
            query.addBindValue(it->first);
            query.addBindValue(QString::fromStdString(it->second));
            if (!query.exec())
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


void DBController::getAvailableSamples(std::map<int, std::string> *samples)
{
    QSqlQuery query;
    if (!query.exec("SELECT participant_id, filename FROM samples"))
        throw std::runtime_error(query.lastError().text().toStdString());

    samples->clear();
    while (query.next()) {
        samples->insert(
            std::pair<int, std::string>(
                query.value(0).toInt(),
                query.value(1).toString().toStdString())
        );
    }
}


void DBController::storeAnnotation(int timestamp, int senderID,
                                   const std::vector<int> &receiverIDs,
                                   const Attributes &attributes)
{
    QSqlQuery query;

    do {
        // Begin a transaction.
        QSqlDatabase::database().transaction();

        query.clear();
        query.prepare("DELETE FROM sender WHERE timestamp = ? AND sender_id = ?");
        query.addBindValue(timestamp);
        query.addBindValue(senderID);
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
        query.prepare("INSERT OR REPLACE INTO attributes "
                      "(timestamp, participant_id, speaking, laughing) "
                      "VALUES (?, ?, ?, ?)");
        query.addBindValue(timestamp);
        query.addBindValue(senderID);
        query.addBindValue(attributes.speaking ? 1 : 0);
        query.addBindValue(attributes.laughing ? 1 : 0);
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


bool DBController::getAnnotation(int timestamp, int senderID,
                                 std::vector<int> *receiverIDs,
                                 Attributes *attributes)
{
    QSqlQuery query;

    // Reset the annotations.
    receiverIDs->clear();
    attributes->speaking = false;
    attributes->laughing = false;

    do {
        QSqlDatabase::database().transaction();

        // Get the IDs of the receivers.
        query.clear();
        query.prepare("SELECT receiver_id FROM sender WHERE timestamp = ? "
                      "AND sender_id = ?");
        query.addBindValue(timestamp);
        query.addBindValue(senderID);
        if (!query.exec())
            break;
        while (query.next())
            receiverIDs->push_back(query.value(0).toInt());

        // Get the stored attributes.
        query.clear();
        query.prepare("SELECT speaking, laughing FROM attributes "
                      "WHERE timestamp = ? AND participant_id = ?");
        query.addBindValue(timestamp);
        query.addBindValue(senderID);
        if (!query.exec())
            break;
        else if (query.next()) {
            attributes->speaking = query.value(0).toBool();
            attributes->laughing = query.value(1).toBool();
        } else {
            // No data available for the given timestamp
            return false;
        }

#if QT_VERSION >= 0x040302
        query.finish();
#else
        while (query.next()) {}
#endif

        // Finalize the transaction.
        if (!QSqlDatabase::database().commit())
            break;

        // Everything's fine, return now.
        return true;
    } while (false);

    // Once we get to this point, an error has occured. Let's throw an
    // exception.
    throw std::runtime_error(query.lastError().text().toStdString());
}


} // namespace annot8
