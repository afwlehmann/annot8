//
// DBController.h
// $Id$
//


#ifndef __DBCONTROLLER_H__
#define __DBCONTROLLER_H__


#include <string>
#include <vector>


namespace hiwi
{


// Forward declaration
struct Movie;
struct Participant;
namespace audio { class Samples; }


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


    /**
     * Returns a list of the available movies.
     */
    std::vector<Movie *> getAvailableMovies() const;


    /**
     * Returns a list of all participants.
     */
    std::vector<Participant *> getParticipants() const;


    /**
     * Returns the samples associated with the given participant.
     * @return              a pointer to a newly allocated Samples object or 0
     */
    audio::Samples* getSamplesForParticipant(Participant *p) const;


    /**
     * Stores the annotation of a frame in terms of the given parameters.
     * @param  timestamp    the exact moment as a real value within [0,1]
     * @param  senderID     the unique ID of the sending participant
     * @param  receiverIDs  a list of the unique IDs of the receivers
     * @param  laughing     whether the sender's laughing or not
     */
    void storeAnnotation(float timestamp, int senderID,
                         const std::vector<int> &receiverIDs,
                         bool laughing);


    /**
     * Retrieves the annotation of a frame for the given timestamp and sender.
     * @param  timestamp    the exact moment as a real value within [0,1]
     * @param  senderID     the unique ID of the sending participant
     * @param  receiverIDs  a pointer to a vector that will hold the IDs of
     *                      of the respective receivers
     * @param  laughing     a pointer to a bool that will tell whether the
     *                      sender was laughing or not
     */
    void getAnnotation(float timestamp, int senderID,
                       std::vector<int> *receiverIDs,
                       bool *laughing);


protected:
    /**
     * Sets up the database by creating the neccessary tables.
     */
    void setupTables();


    /**
     * Sets up the database by creating the neccessary triggers. This method
     * does a little bit of `magic' and is thus very dependent on SQLite.
     */
    void setupTriggers();


private:
    // Forbid default constructor etc. due to DBController being a singleton.
    DBController();
    DBController(const DBController&);
    DBController& operator=(const DBController&);


    static DBController    *_instance;
};


} // namespace hiwi


#endif
