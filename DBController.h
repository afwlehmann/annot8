//
// DBController.h
// $Id$
//


#ifndef __DBCONTROLLER_H__
#define __DBCONTROLLER_H__


#include <string>
#include <vector>
#include <map>


namespace hiwi
{


// Forward declaration
struct Movie;
struct Participant;
struct Attributes;
namespace audio { struct Samples; }


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
     * @param  withSamplesOnly  get only those participants with related samples
     */
    std::vector<Participant *> getParticipants(bool withSamplesOnly = false) const;


    /**
     * Retrieves the available participants info strings.
     * @param  pinfo        a pointer to a map that will hold the infos
     */
    void getParticipantsInfo(std::map<int, std::string> *info);


    /**
     * Returns the samples associated with the given participant.
     * @return              a pointer to a newly allocated Samples object or 0
     */
    audio::Samples* getSamplesForParticipant(Participant *p) const;


    /**
     * Stores the available samples.
     * @param  samples      a map from participant ids to filenames
     */
    void storeAvailableSamples(std::map<int, std::string> &samples);


    /**
     * Retrieves the available samples.
     * @param  samples      a pointer to a map from participant ids to filenames
     */
    void getAvailableSamples(std::map<int, std::string> *samples);


    /**
     * Stores the annotation of a frame in terms of the given parameters.
     * @param  timestamp    the timestamp in milliseconds
     * @param  senderID     the unique ID of the sending participant
     * @param  receiverIDs  a list of the unique IDs of the receivers
     * @param  attributes   the attributes for the corresponding frame
     */
    void storeAnnotation(int timestamp, int senderID,
                         const std::vector<int> &receiverIDs,
                         const Attributes &attributes);


    /**
     * Retrieves the annotation of a frame for the given timestamp and sender.
     * @param  timestamp    the timestamp in milliseconds
     * @param  senderID     the unique ID of the sending participant
     * @param  receiverIDs  a pointer to a vector that will hold the IDs of
     *                      of the respective receivers
     * @param  attributes   a pointer to the structure that will hold the
     *                      attributes of the corresponding frame
     * @return              true iff data could be retrieved for the given
     *                      timestamp
     */
    bool getAnnotation(int timestamp, int senderID,
                       std::vector<int> *receiverIDs,
                       Attributes *attributes);


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
