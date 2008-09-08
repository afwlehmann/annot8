//
// DBController.h
// $Id$
//


#ifndef __DBCONTROLLER_H__
#define __DBCONTROLLER_H__


#include <string>


namespace HiWi
{


class DBController
{
public:
    virtual ~DBController();

    /**
     * Returns the one and only instance of the database controller. Creates
     * this instance iff neccessary.
     * @return      an instance of the database controller
     */
    static DBController* instance();


    /**
     * Connects the database controller to a given SQLite database.
     * @param  fileName     the name of the SQLite database file
     * @return              whether the connection attempt succeeded
     */
    bool connect(const std::string &fileName);


    /**
     * Disconnects the database controller from a prior connected database.
     */
    void disconnect();


private:
    // Forbid default constructor due to DBController being a singleton.
    DBController();


    static DBController    *_instance;
};


} // namespace HiWi


#endif
